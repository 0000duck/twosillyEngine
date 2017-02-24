#include "Processor.h"
#include "multiVolumes.h"

namespace Engine {

	
	/**
	时间：2016年8月4日 11:34:45
	内容：gui的链接，与gui即服务器建立链接
			注：这是网络编程的，我打算摒弃它
		参数：
			int portNr --> 端口号

	作者：TwoSilly
	*/
	void Processor::guiConnect(int portNr)
	{
		_guiSocket.connectTo("127.0.0.1", portNr);
	}

	/**
	时间：2016年10月31日10:45:34
	内容：利用套接字，发送矩形给GUI
		参数：
			const char* name --> 当前矩形的名字（支撑，填充等）
			int layerNr --> 层编码
			int32_t z --> z轴大小
			Polygons& polygons --> 矩形数据

	作者：TwoSilly
	*/
	void Processor::sendPolygonsToGui(const char* name, int layerNr, int32_t z, Polygons& polygons)
	{
		//Engine::logError("发送矩形信息，等待gui接收数据！\n");
		_guiSocket.sendNr(GUI_CMD_SEND_POLYGONS);
		_guiSocket.sendNr(polygons.size());
		_guiSocket.sendNr(layerNr);
		_guiSocket.sendNr(z);
		_guiSocket.sendNr(strlen(name));
		_guiSocket.sendAll(name, strlen(name));
		for (unsigned int n = 0; n < polygons.size(); n++)
		{
			PolygonRef polygon = polygons[n];
			_guiSocket.sendNr(polygon.size());
			_guiSocket.sendAll(polygon.data(), polygon.size() * sizeof(Point));
		}
	}

	/**
	时间：2016年8月4日 11:34:45
	内容：设置目标文件
		参数：
			const char* filename -->

	作者：TwoSilly
	*/
	bool Processor::setTargetFile(const char* filename)
	{
		_gcode.setFilename(filename);
		if (_gcode.isOpened())
			_gcode.writeComment("Generated with Engine %s", VERSION);
		return _gcode.isOpened();
	}

	/**
	时间：2016年8月4日 11:34:45
	内容：过程文件
		参数：
			const std::vector<std::string> &files -->

	作者：TwoSilly
	*/
	bool Processor::processFile(const std::vector<std::string> &files)
	{
		if (!_gcode.isOpened())
			return false;

		TimeKeeper timeKeeperTotal;
		SliceDataStorage storage;
		preSetup();



		if (!prepareModel(storage, files))
			return false;

		processSliceData(storage);
		//dumpLayerparts(storage, "I:/TwoSillyEngine/TwoSillyEngine/processSliceData.html");
	

		
		writeGCode(storage);
		//dumpLayerparts(storage, "I:/TwoSillyEngine/TwoSillyEngine/writeGCode.html");
		


		Engine::logProgress("process", 1, 1);//.报告一个文件已被完全处理的图形用户界面
		Engine::log("Total time elapsed %5.2fs.\n", timeKeeperTotal.restart());
		//我打算改了这些,然而我放弃了，发现这东西貌似留着更好
		_guiSocket.sendNr(GUI_CMD_FINISH_OBJECT);

		return true;
	}

	
	/**
	时间：2016年8月4日 11:34:45
	内容：结束文件，跟析构函数类似的最后才调用的函数
		参数：

	作者：TwoSilly
	*/
	void Processor::finalize()
	{
		if (!_gcode.isOpened())
			return;
		_gcode.finalize(_maxObjectHeight, _config.moveSpeed, _config.endCode.c_str());
	}

	/**
	时间：2016年8月4日 11:34:45
	内容：预先的计划
		参数：

	作者：TwoSilly
	*/
	void Processor::preSetup()
	{
		_skirtConfig.setData(_config.printSpeed, _config.extrusionWidth, "SKIRT");
		_inset0Config.setData(_config.inset0Speed, _config.extrusionWidth, "WALL-OUTER");
		_insetXConfig.setData(_config.insetXSpeed, _config.extrusionWidth, "WALL-INNER");
		_infillConfig.setData(_config.infillSpeed, _config.extrusionWidth, "FILL");
		_skinConfig.setData(_config.skinSpeed, _config.extrusionWidth, "FILL");
		_supportConfig.setData(_config.printSpeed, _config.extrusionWidth, "SUPPORT");

		for(unsigned int n=1; n<MAX_EXTRUDERS;n++)
			_gcode.setExtruderOffset(n, _config.extruderOffset[n].p());

		_gcode.setSwitchExtruderCode(_config.preSwitchExtruderCode, _config.postSwitchExtruderCode);
		_gcode.setFlavor(_config.gcodeFlavor);
		_gcode.setRetractionSettings(_config.retractionAmount,
									_config.retractionSpeed,
									_config.retractionAmountExtruderSwitch,
									_config.minimalExtrusionBeforeRetraction,
									_config.retractionZHop,
									_config.retractionAmountPrime);
		_gcode.applyAccelerationSettings(_config);

	}

	/*
	时间：2016年10月28日16:43:52
	内容：准备模型，从gui接收模型
	作者：TwoSilly
	*/
	bool Processor::prepareModel(SliceDataStorage& storage, const std::vector<std::string> &files)
	{

		_timeKeeper.restart();
		IntModelFile* model = nullptr;
		if (files.size() == 1 && files[0][0] == '$')
		{
			
			const char *input_filename = files[0].c_str();
			model = new IntModelFile();
			for(unsigned int n=0; input_filename[n]; n++)
			{
				model->volumes.push_back(IntFaceVolume());
				IntFaceVolume* volume = &model->volumes[model->volumes.size()-1];
				Engine::logError("发送网格信息，等待gui传输数据！\n");
				_guiSocket.sendNr(GUI_CMD_REQUEST_MESH);//这里发送给gui后就卡住了、、

				int32_t vertexCount = _guiSocket.recvNr();
				Engine::logError("接收到：！%d \n",vertexCount);//、程序没走到这里？是的
				//Engine::logError("你打野:%d\n",vertexCount);
				int pNr = 0;
				Engine::log("Reading mesh from socket with %i vertexes\n", vertexCount);
				Point3 v[3];
				/*
				时间：2016年9月27日16:23:29
				内容：这是为了得到gui传入的数据值，发现当他与gui链接时，gui会把模型转换为点传入engine 
				作者：TwoSilly
				*/
				// ocut;
				//ocut.open("_guiSocket.txt");
				//ofstream ocutv;
				//ocutv.open("gui.txt");

				while(vertexCount)
				{
					float f[3];
					_guiSocket.recvAll(f, 3 * sizeof(float));
					//Engine::logError("RECV:%d-->(%f,%f,%f)\n",vertexCount,f[0],f[1],f[2]);
					//ocut<<"vertexCount:"<<vertexCount<<"--> ("<<f[0]<<","<<f[1]<<","<<f[2]<<")"<<"\n";

					FloatPoint3 fp(f[0], f[1], f[2]);
					v[pNr++] = _config.matrix.apply(fp);
					if (pNr == 3)
					{
						volume->addFace(v[0], v[1], v[2]);
						//ocutv<<"vertexCount:"<<vertexCount<<"--> ("<<v[0].x<<","<<v[0].y<<","<<v[0].z<<")"<<"\n";
						//ocutv<<"vertexCount:"<<vertexCount<<"--> ("<<v[1].x<<","<<v[1].y<<","<<v[1].z<<")"<<"\n";
						//ocutv<<"vertexCount:"<<vertexCount<<"--> ("<<v[2].x<<","<<v[2].y<<","<<v[2].z<<")"<<"\n";
						pNr = 0;
					}
					vertexCount--;
				}

				//ocut.close();
				//ocutv.close();
			}
		}else{
			model = new IntModelFile();
			for(unsigned int i=0;i < files.size(); i++) {
				if(files[i] == "-")
					model->volumes.push_back(IntFaceVolume());
				else {
					Engine::log("Loading %s from disk...\n", files[i].c_str());
					IntModelFile *test = loadModelFromFile(model,files[i].c_str(), _config.matrix);
					if(test == nullptr) { // error while reading occurred读取发生时出错
						Engine::logError("Failed to load model: %s\n", files[i].c_str());
						return false;
					}
				}
			}
		}
		Engine::log("Loaded from disk in %5.3fs\n", _timeKeeper.restart());
		Engine::log("Analyzing and optimizing model...\n");
		OptimizedModel* optimizedModel = new OptimizedModel(model, Point3(_config.objectPosition.X_, _config.objectPosition.Y_, -_config.objectSink));
		for(unsigned int v = 0; v < model->volumes.size(); v++)
		{
			Engine::log("  Face counts: %i -> %i %0.1f%%\n", (int)model->volumes[v].faces.size(), (int)optimizedModel->volumes_[v].faces.size(), float(optimizedModel->volumes_[v].faces.size()) / float(model->volumes[v].faces.size()) * 100);
			Engine::log("  Vertex counts: %i -> %i %0.1f%%\n", (int)model->volumes[v].faces.size() * 3, (int)optimizedModel->volumes_[v].points.size(), float(optimizedModel->volumes_[v].points.size()) / float(model->volumes[v].faces.size() * 3) * 100);
			Engine::log("  Size: %f %f %f\n", INT2MM(optimizedModel->modelSize_.x), INT2MM(optimizedModel->modelSize_.y), INT2MM(optimizedModel->modelSize_.z));
			Engine::log("  vMin: %f %f %f\n", INT2MM(optimizedModel->vMin_.x), INT2MM(optimizedModel->vMin_.y), INT2MM(optimizedModel->vMin_.z));
			Engine::log("  vMax: %f %f %f\n", INT2MM(optimizedModel->vMax_.x), INT2MM(optimizedModel->vMax_.y), INT2MM(optimizedModel->vMax_.z));
			Engine::log("  vMin: %f %f %f\n", INT2MM(model->Min().x), INT2MM(model->Min().y), INT2MM(model->Min().z));
			Engine::log("  vMax: %f %f %f\n", INT2MM(model->Max().x), INT2MM(model->Max().y), INT2MM(model->Max().z));
			Engine::log("  Matrix: %f %f %f\n", _config.matrix.m[0][0], _config.matrix.m[1][0], _config.matrix.m[2][0]);
			Engine::log("  Matrix: %f %f %f\n", _config.matrix.m[0][1], _config.matrix.m[1][1], _config.matrix.m[2][1]);
			Engine::log("  Matrix: %f %f %f\n", _config.matrix.m[0][2], _config.matrix.m[1][2], _config.matrix.m[2][2]);
			if (INT2MM(optimizedModel->modelSize_.x) > 10000.0 || INT2MM(optimizedModel->modelSize_.y)  > 10000.0 || INT2MM(optimizedModel->modelSize_.z) > 10000.0)
			{
				Engine::logError("Object is way to big, CuraEngine bug?");
				exit(1);
			}
		}
		delete model;
		Engine::log("Optimize model %5.3fs \n", _timeKeeper.restart());
		//om->saveDebugSTL("c:\\models\\output.stl");

		Engine::log("Slicing model...\n");
		vector<Slicer*> slicerList;
		for(unsigned int volumeIdx=0; volumeIdx < optimizedModel->volumes_.size(); volumeIdx++)
		{
			Slicer* slicer = new Slicer(&optimizedModel->volumes_[volumeIdx], _config.initialLayerThickness - _config.layerThickness / 2, _config.layerThickness, _config.fixHorrible & FIX_HORRIBLE_KEEP_NONE_CLOSED, _config.fixHorrible & FIX_HORRIBLE_EXTENSIVE_STITCHING);
			slicerList.push_back(slicer);
			for(unsigned int layerNr=0; layerNr<slicer->layers.size(); layerNr++)
			{
				
				//报告大纲这里减慢了引擎相当一点，所以只有在调试时这样做。
				//sendPolygonsToGui("outline", layerNr, slicer->layers[layerNr].z, slicer->layers[layerNr].polygonList);


				sendPolygonsToGui("openoutline", layerNr, slicer->layers[layerNr].z_, slicer->layers[layerNr].openPolygons_);
			}
		}
		Engine::log("Sliced model in %5.3fs\n", _timeKeeper.restart());

		Engine::log("Generating support map...\n");
		generateSupportGrid(storage.support, optimizedModel, _config.supportAngle, _config.supportEverywhere > 0, _config.supportXYDistance, _config.supportZDistance);

		storage.modelSize = optimizedModel->modelSize_;
		storage.modelMin = optimizedModel->vMin_;
		storage.modelMax = optimizedModel->vMax_;
		delete optimizedModel;

		Engine::log("Generating layer parts...\n");
		for(unsigned int volumeIdx=0; volumeIdx < slicerList.size(); volumeIdx++)
		{
			storage.volumes.push_back(SliceVolumeStorage());
			createLayerParts(storage.volumes[volumeIdx], slicerList[volumeIdx], _config.fixHorrible & (FIX_HORRIBLE_UNION_ALL_TYPE_A | FIX_HORRIBLE_UNION_ALL_TYPE_B | FIX_HORRIBLE_UNION_ALL_TYPE_C));
			delete slicerList[volumeIdx];
			slicerList[volumeIdx] = nullptr;

			/*
			时间：2016年12月16日14:28:35
			内容：这里有可以优化的地方，
					当没有底筏时这里不需要加偏移量！
					不过貌似没有什么效果！郁闷！不过我还是决定把它留着吧！不可能没用才对
			作者：TwoSilly
			  */
			//将筏板偏移到每一层。
				//当这两个数不等零 就不需要计算
			if(_config.raftBaseThickness + _config.raftInterfaceThickness){
				for(unsigned int layerNr=0; layerNr<storage.volumes[volumeIdx].layers_.size(); layerNr++)
					storage.volumes[volumeIdx].layers_[layerNr].printZ += _config.raftBaseThickness + _config.raftInterfaceThickness;
			}
		}
		Engine::log("Generated layer parts in %fs\n", _timeKeeper.restart());
		return true;
	}

	void Processor::processSliceData(SliceDataStorage& storage)
	{
			//总计层 （我觉得是初始层计数）
		const unsigned int totalLayers = storage.volumes[0].layers_.size();

		//carveMultipleVolumes(storage.volumes);
		generateMultipleVolumesOverlap(storage.volumes, _config.multiVolumeOverlap);
		
		//dumpLayerparts(storage, "E:/task/TwoSillyEngine/TwoSillyEngine/output.html");
		if (_config.simpleMode)
		{
			for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
			{
				for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
				{
					SliceLayer* layer = &storage.volumes[volumeIdx].layers_[layerNr];
					for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
					{
						sendPolygonsToGui("inset0", layerNr, layer->printZ, layer->parts[partNr].outline);
					}
				}
			}
			return;
		}

		for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
		{
			for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
			{
				int insetCount = _config.insetCount;        
				
				/*
				时间：2016年12月16日15:29:22
				内容：这是旋转打印，每两层，就是旋转上升这是为了排除，同一点上升出现的圆线
				作者：
				*/
				if (_config.spiralizeMode && static_cast<int>(layerNr) < _config.downSkinCount && layerNr % 2 == 1)
					insetCount += 5;
				SliceLayer* layer = &storage.volumes[volumeIdx].layers_[layerNr];
				int extrusionWidth = _config.extrusionWidth;
				if (layerNr == 0)
					extrusionWidth = _config.layer0extrusionWidth;

				generateInsets(layer, extrusionWidth, insetCount);

				for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
				{
					if (layer->parts[partNr].insets.size() > 0)
					{
						sendPolygonsToGui("inset0", layerNr, layer->printZ, layer->parts[partNr].insets[0]);
						for(unsigned int inset=1; inset<layer->parts[partNr].insets.size(); inset++)
							sendPolygonsToGui("insetx", layerNr, layer->printZ, layer->parts[partNr].insets[inset]);
					}
				}
			}
			Engine::logProgress("inset",layerNr+1,totalLayers);
		}
		if (_config.enableOozeShield)
		{
			for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
			{
				Polygons oozeShield;
				for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
				{
					for(unsigned int partNr=0; partNr<storage.volumes[volumeIdx].layers_[layerNr].parts.size(); partNr++)
					{
						oozeShield = oozeShield.unionPolygons(storage.volumes[volumeIdx].layers_[layerNr].parts[partNr].outline.offset(MM2INT(2.0)));
					}
				}
				storage.oozeShield.push_back(oozeShield);
			}

			for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
				storage.oozeShield[layerNr] = storage.oozeShield[layerNr].offset(-MM2INT(1.0)).offset(MM2INT(1.0));

			int offsetAngle = static_cast<int>(tan(60.0*M_PI/180) * _config.layerThickness);//允许在软泥中屏蔽一个60度角。
			for(unsigned int layerNr=1; layerNr<totalLayers; layerNr++)
				storage.oozeShield[layerNr] = storage.oozeShield[layerNr].unionPolygons(storage.oozeShield[layerNr-1].offset(-offsetAngle));
			for(unsigned int layerNr=totalLayers-1; layerNr>0; layerNr--)
				storage.oozeShield[layerNr-1] = storage.oozeShield[layerNr-1].unionPolygons(storage.oozeShield[layerNr].offset(-offsetAngle));
		}
		Engine::log("Generated inset in %5.3fs\n", _timeKeeper.restart());

		for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
		{
			if (!_config.spiralizeMode || static_cast<int>(layerNr) < _config.downSkinCount)    //只有产生了/ downskin和填充的第X层当spiralize选择。
			{
				for(unsigned int volumeIdx=0; volumeIdx<storage.volumes.size(); volumeIdx++)
				{
					int extrusionWidth = _config.extrusionWidth;
					if (layerNr == 0)
						extrusionWidth = _config.layer0extrusionWidth;
					generateSkins(layerNr, storage.volumes[volumeIdx], extrusionWidth, _config.downSkinCount, _config.upSkinCount, _config.infillOverlap);
					generateSparse(layerNr, storage.volumes[volumeIdx], extrusionWidth, _config.downSkinCount, _config.upSkinCount);

					SliceLayer* layer = &storage.volumes[volumeIdx].layers_[layerNr];
					for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
						sendPolygonsToGui("skin", layerNr, layer->printZ, layer->parts[partNr].skinOutline);
				}
			}
			Engine::logProgress("skin",layerNr+1,totalLayers);
		}
		Engine::log("Generated up/down skin in %5.3fs\n", _timeKeeper.restart());

		if (_config.wipeTowerSize > 0)
		{
			PolygonRef p = storage.wipeTower.newPoly();
			p.add(Point(storage.modelMin.x - 3000, storage.modelMax.y + 3000));
			p.add(Point(storage.modelMin.x - 3000, storage.modelMax.y + 3000 + _config.wipeTowerSize));
			p.add(Point(storage.modelMin.x - 3000 - _config.wipeTowerSize, storage.modelMax.y + 3000 + _config.wipeTowerSize));
			p.add(Point(storage.modelMin.x - 3000 - _config.wipeTowerSize, storage.modelMax.y + 3000));

			storage.wipePoint = Point(storage.modelMin.x - 3000 - _config.wipeTowerSize / 2, storage.modelMax.y + 3000 + _config.wipeTowerSize / 2);
		}

		if (_config.raftBaseThickness > 0 && _config.raftInterfaceThickness > 0)
			generateSkirt(storage, _config.raftMargin + _config.raftBaseLinewidth, _config.raftBaseLinewidth, _config.skirtLineCount, _config.skirtMinLength, _config.raftBaseThickness);
		else
			generateSkirt(storage, _config.skirtDistance, _config.layer0extrusionWidth, _config.skirtLineCount, _config.skirtMinLength, _config.initialLayerThickness);
		generateRaft(storage, _config.raftMargin);

		sendPolygonsToGui("skirt", 0, _config.initialLayerThickness, storage.skirt);
	}

	void Processor::writeGCode(SliceDataStorage& storage)
	{
		
		//dumpLayerparts(storage, "I:/TwoSillyEngine/TwoSillyEngine/writeGCode.html");

		if (_fileNr == 1)
		{
			if (_gcode.getFlavor() == GCODE_FLAVOR_ULTIGCODE)
			{
				_gcode.writeComment("FLAVOR:UltiGCode");
				_gcode.writeComment("TIME:<__TIME__>");
				_gcode.writeComment("MATERIAL:<FILAMENT>");
				_gcode.writeComment("MATERIAL2:<FILAMEN2>");
				_gcode.writeComment("NOZZLE_DIAMETER:%f", float(_config.nozzleSize) / 1000);
				_gcode.writeComment("NOZZLE_DIAMETER2:%f", float(_config.nozzleSize) / 1000);
			}
			_gcode.writeCode(_config.startCode.c_str());
			/**
			时间：2016年8月7日 08:57:35
			内容：这是我为了测试加的！
				Engine::log( "startCode:",_config.startCode.c_str());
			作者：TwoSilly
			*/
			Engine::log( "startCode:",_config.startCode.c_str());
			if (_gcode.getFlavor() == GCODE_FLAVOR_BFB)
			{
				_gcode.writeComment("enable auto-retraction");
				_gcode.writeLine("M227 S%d P%d", _config.retractionAmount * 2560 / 1000, _config.retractionAmount * 2560 / 1000);
			}
		}else{
			_gcode.writeFanCommand(0);
			_gcode.resetExtrusionValue();
			_gcode.writeRetraction();
			_gcode.setZ(_maxObjectHeight + 5000);
			_gcode.writeMove(_gcode.getPositionXY(), _config.moveSpeed, 0);
			_gcode.writeMove(Point(storage.modelMin.x, storage.modelMin.y), _config.moveSpeed, 0);
		}
		_fileNr++;

		unsigned int totalLayers = storage.volumes[0].layers_.size();
		_gcode.writeComment("Layer count: %d", totalLayers);

		if (_config.raftBaseThickness > 0 && _config.raftInterfaceThickness > 0)
		{
			sendPolygonsToGui("support", 0, _config.raftBaseThickness, storage.raftOutline);
			sendPolygonsToGui("support", 0, _config.raftBaseThickness + _config.raftInterfaceThickness, storage.raftOutline);

			GCodePathConfig raftBaseConfig((_config.raftBaseSpeed <= 0) ? _config.initialLayerSpeed : _config.raftBaseSpeed, _config.raftBaseLinewidth, "SUPPORT");
			
			GCodePathConfig raftMiddleConfig((_config.raftBaseSpeed <= 0) ? _config.initialLayerSpeed : _config.raftBaseSpeed, _config.raftInterfaceLinewidth, "SUPPORT");
			
			GCodePathConfig raftInterfaceConfig((_config.raftBaseSpeed <= 0) ? _config.initialLayerSpeed : _config.raftBaseSpeed, _config.raftInterfaceLinewidth, "SUPPORT");
			
			GCodePathConfig raftSurfaceConfig((_config.raftSurfaceSpeed > 0) ? _config.raftSurfaceSpeed : _config.printSpeed, _config.raftSurfaceLinewidth, "SUPPORT");

			{
				_gcode.writeComment("LAYER:-2");
				_gcode.writeComment("RAFT");
				GCodePlanner gcodeLayer(_gcode, _config.moveSpeed, _config.retractionMinimalDistance);
				if (_config.supportExtruder > 0)
					gcodeLayer.setExtruder(_config.supportExtruder);
				_gcode.setZ(_config.raftBaseThickness);
				_gcode.setExtrusion(_config.raftBaseThickness, _config.filamentDiameter, _config.filamentFlow);

				gcodeLayer.addPolygonsByOptimizer(storage.skirt, &raftBaseConfig);
				gcodeLayer.addPolygonsByOptimizer(storage.raftOutline, &raftBaseConfig);

				Polygons raftLines;
				generateLineInfill(storage.raftOutline, raftLines, _config.raftBaseLinewidth, _config.raftLineSpacing, _config.infillOverlap, 0);
				gcodeLayer.addPolygonsByOptimizer(raftLines, &raftBaseConfig);

				gcodeLayer.writeGCode(false, _config.raftBaseThickness);
			}

			if (_config.raftFanSpeed) {
				_gcode.writeFanCommand(_config.raftFanSpeed);
			}

			{
				_gcode.writeComment("LAYER:-1");
				_gcode.writeComment("RAFT");
				GCodePlanner gcodeLayer(_gcode, _config.moveSpeed, _config.retractionMinimalDistance);
				_gcode.setZ(_config.raftBaseThickness + _config.raftInterfaceThickness);
				_gcode.setExtrusion(_config.raftInterfaceThickness, _config.filamentDiameter, _config.filamentFlow);

				Polygons raftLines;
				generateLineInfill(storage.raftOutline, raftLines, _config.raftInterfaceLinewidth, _config.raftInterfaceLineSpacing, _config.infillOverlap, _config.raftSurfaceLayers > 0 ? 45 : 90);
				gcodeLayer.addPolygonsByOptimizer(raftLines, &raftInterfaceConfig);

				gcodeLayer.writeGCode(false, _config.raftInterfaceThickness);
			}

			for (int raftSurfaceLayer=1; raftSurfaceLayer<=_config.raftSurfaceLayers; raftSurfaceLayer++)
			{
				_gcode.writeComment("LAYER:-1");
				_gcode.writeComment("RAFT");
				GCodePlanner gcodeLayer(_gcode, _config.moveSpeed, _config.retractionMinimalDistance);
				_gcode.setZ(_config.raftBaseThickness + _config.raftInterfaceThickness + _config.raftSurfaceThickness*raftSurfaceLayer);
				_gcode.setExtrusion(_config.raftSurfaceThickness, _config.filamentDiameter, _config.filamentFlow);

				Polygons raftLines;
				generateLineInfill(storage.raftOutline, raftLines, _config.raftSurfaceLinewidth, _config.raftSurfaceLineSpacing, _config.infillOverlap, 90 * raftSurfaceLayer);
				gcodeLayer.addPolygonsByOptimizer(raftLines, &raftSurfaceConfig);

				gcodeLayer.writeGCode(false, _config.raftInterfaceThickness);
			}
		}

		int volumeIdx = 0;
		for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
		{
			Engine::logProgress("export", layerNr+1, totalLayers);

			int extrusionWidth = _config.extrusionWidth;
			if (layerNr == 0)
				extrusionWidth = _config.layer0extrusionWidth;
			if (static_cast<int>(layerNr) < _config.initialSpeedupLayers)
			{
				int n = _config.initialSpeedupLayers;
#define SPEED_SMOOTH(speed) \
	std::min<int>((speed), (((speed)*layerNr)/n + (_config.initialLayerSpeed*(n-layerNr)/n)))

				_skirtConfig.setData(SPEED_SMOOTH(_config.printSpeed), extrusionWidth, "SKIRT");
				_inset0Config.setData(SPEED_SMOOTH(_config.inset0Speed), extrusionWidth, "WALL-OUTER");
				_insetXConfig.setData(SPEED_SMOOTH(_config.insetXSpeed), extrusionWidth, "WALL-INNER");
				_infillConfig.setData(SPEED_SMOOTH(_config.infillSpeed), extrusionWidth,  "FILL");
				_skinConfig.setData(SPEED_SMOOTH(_config.skinSpeed), extrusionWidth,  "SKIN");
				_supportConfig.setData(SPEED_SMOOTH(_config.printSpeed), extrusionWidth, "SUPPORT");
#undef SPEED_SMOOTH

			}else{
				_skirtConfig.setData(_config.printSpeed, extrusionWidth, "SKIRT");
				_inset0Config.setData(_config.inset0Speed, extrusionWidth, "WALL-OUTER");
				_insetXConfig.setData(_config.insetXSpeed, extrusionWidth, "WALL-INNER");
				_infillConfig.setData(_config.infillSpeed, extrusionWidth, "FILL");
				_skinConfig.setData(_config.skinSpeed, extrusionWidth, "SKIN");
				_supportConfig.setData(_config.printSpeed, extrusionWidth, "SUPPORT");
			}

			_gcode.writeComment("LAYER:%d", layerNr);
			if (layerNr == 0)
				_gcode.setExtrusion(_config.initialLayerThickness, _config.filamentDiameter, _config.filamentFlow);
			else
				_gcode.setExtrusion(_config.layerThickness, _config.filamentDiameter, _config.filamentFlow);

			GCodePlanner gcodeLayer(_gcode, _config.moveSpeed, _config.retractionMinimalDistance);
			int32_t z = _config.initialLayerThickness + layerNr * _config.layerThickness;
			z += _config.raftBaseThickness + _config.raftInterfaceThickness + _config.raftSurfaceLayers*_config.raftSurfaceThickness;
			if (_config.raftBaseThickness > 0 && _config.raftInterfaceThickness > 0)
			{
				if (layerNr == 0)
				{
					z += _config.raftAirGapLayer0;
				} else {
					z += _config.raftAirGap;
				}
			}
			_gcode.setZ(z);
			_gcode.resetStartPosition();

			bool printSupportFirst = (storage.support.generated && _config.supportExtruder > 0 && _config.supportExtruder == gcodeLayer.getExtruder());
			if (printSupportFirst)
				addSupportToGCode(storage, gcodeLayer, layerNr);

			for(unsigned int volumeCnt = 0; volumeCnt < storage.volumes.size(); volumeCnt++)
			{
				if (volumeCnt > 0)
					volumeIdx = (volumeIdx + 1) % storage.volumes.size();
				addVolumeLayerToGCode(storage, gcodeLayer, volumeIdx, layerNr);
			}
			if (!printSupportFirst)
				addSupportToGCode(storage, gcodeLayer, layerNr);

			//Finish the layer by applying speed corrections for minimal layer times
			//通过应用最小层时间的速度修正来完成这一层
			gcodeLayer.forceMinimalLayerTime(_config.minimalLayerTime, _config.minimalFeedrate);

			int fanSpeed = _config.fanSpeedMin;
			if (gcodeLayer.getExtrudeSpeedFactor() <= 50)
			{
				fanSpeed = _config.fanSpeedMax;
			}else{
				int n = gcodeLayer.getExtrudeSpeedFactor() - 50;
				fanSpeed = _config.fanSpeedMin * n / 50 + _config.fanSpeedMax * (50 - n) / 50;
			}
			if (static_cast<int>(layerNr) < _config.fanFullOnLayerNr)
			{
				//Slow down the fan on the layers below the [fanFullOnLayerNr], where layer 0 is speed 0.
				//慢下来的风扇在层下面的[ fanfullonlayernr ]，其中0层是速度0
				fanSpeed = fanSpeed * layerNr / _config.fanFullOnLayerNr;
			}
			_gcode.writeFanCommand(fanSpeed);

			gcodeLayer.writeGCode(_config.coolHeadLift > 0, static_cast<int>(layerNr) > 0 ? _config.layerThickness : _config.initialLayerThickness);
		}

		Engine::log("Wrote layers in %5.2fs.\n", _timeKeeper.restart());
		_gcode.tellFileSize();
		_gcode.writeFanCommand(0);

		//当我们打印多个对象时，存储对象的高度，因为我们需要在移动到下一个位置时清除它们的每一个。
		_maxObjectHeight = (std::max)(_maxObjectHeight, storage.modelSize.z - _config.objectSink);
	}

		/**
	时间：2016年8月4日 11:34:45
	内容：添加层的体积到Gocde，从一个单一的网格体积的Gocde添加一个层
		参数：
		SliceDataStorage& storage -->
		GCodePlanner& gcodeLayer -->
		int volumeIdx -->
		int layerNr -->
	作者：TwoSilly
	*/
	void Processor::addVolumeLayerToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int volumeIdx, int layerNr)
	{
		int prevExtruder = gcodeLayer.getExtruder();
		bool extruderChanged = gcodeLayer.setExtruder(volumeIdx);
		if (layerNr == 0 && volumeIdx == 0 && !(_config.raftBaseThickness > 0 && _config.raftInterfaceThickness > 0))
		{
			if (storage.skirt.size() > 0)
				gcodeLayer.addTravel(storage.skirt[storage.skirt.size()-1].closestPointTo(_gcode.getPositionXY()));
			gcodeLayer.addPolygonsByOptimizer(storage.skirt, &_skirtConfig);
		}

		SliceLayer* layer = &storage.volumes[volumeIdx].layers_[layerNr];
		if (extruderChanged)
			addWipeTower(storage, gcodeLayer, layerNr, prevExtruder);

		if (storage.oozeShield.size() > 0 && storage.volumes.size() > 1)
		{
			gcodeLayer.setAlwaysRetract(true);
			gcodeLayer.addPolygonsByOptimizer(storage.oozeShield[layerNr], &_skirtConfig);
			sendPolygonsToGui("oozeshield", layerNr, layer->printZ, storage.oozeShield[layerNr]);
			gcodeLayer.setAlwaysRetract(_config.enableCombing == COMBING_OFF);
		}

		if (_config.simpleMode)
		{
			Polygons polygons;
			for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
			{
				for(unsigned int n=0; n<layer->parts[partNr].outline.size(); n++)
				{
					for(unsigned int m=1; m<layer->parts[partNr].outline[n].size(); m++)
					{
						Polygon p;
						p.add(layer->parts[partNr].outline[n][m-1]);
						p.add(layer->parts[partNr].outline[n][m]);
						polygons.add(p);
					}
					if (layer->parts[partNr].outline[n].size() > 0)
					{
						Polygon p;
						p.add(layer->parts[partNr].outline[n][layer->parts[partNr].outline[n].size()-1]);
						p.add(layer->parts[partNr].outline[n][0]);
						polygons.add(p);
					}
				}
			}
			for(unsigned int n=0; n<layer->openLines.size(); n++)
			{
				for(unsigned int m=1; m<layer->openLines[n].size(); m++)
				{
					Polygon p;
					p.add(layer->openLines[n][m-1]);
					p.add(layer->openLines[n][m]);
					polygons.add(p);
				}
			}
			if (_config.spiralizeMode)
				_inset0Config.spiralize = true;

			gcodeLayer.addPolygonsByOptimizer(polygons, &_inset0Config);
			return;
		}


		PathOrderOptimizer partOrderOptimizer(_gcode.getStartPositionXY());
		for(unsigned int partNr=0; partNr<layer->parts.size(); partNr++)
		{
			partOrderOptimizer.addPolygon(layer->parts[partNr].insets[0][0]);
		}
		partOrderOptimizer.optimize();

		for(unsigned int partCounter=0; partCounter<partOrderOptimizer.polyOrder_.size(); partCounter++)
		{
			SliceLayerPart* part = &layer->parts[partOrderOptimizer.polyOrder_[partCounter]];

			if (_config.enableCombing == COMBING_OFF)
			{
				gcodeLayer.setAlwaysRetract(true);
			}else
			{
				gcodeLayer.setCombBoundary(&part->combBoundery);
				gcodeLayer.setAlwaysRetract(false);
			}

			int fillAngle = 45;
			if (layerNr & 1)
				fillAngle += 90;
			int extrusionWidth = _config.extrusionWidth;
			if (layerNr == 0)
				extrusionWidth = _config.layer0extrusionWidth;

			// Add either infill or perimeter first depending on option添加填充或周边首先取决于选择
			if (!_config.perimeterBeforeInfill) 
			{
				addInfillToGCode(part, gcodeLayer, layerNr, extrusionWidth, fillAngle);
				addInsetToGCode(part, gcodeLayer, layerNr);
			}else
			{
				addInsetToGCode(part, gcodeLayer, layerNr);
				addInfillToGCode(part, gcodeLayer, layerNr, extrusionWidth, fillAngle);
			}

			Polygons skinPolygons;
			for(Polygons outline : part->skinOutline.splitIntoParts())
			{
				int bridge = -1;
				if (layerNr > 0)
					bridge = bridgeAngle(outline, &storage.volumes[volumeIdx].layers_[layerNr-1]);
				generateLineInfill(outline, skinPolygons, extrusionWidth, extrusionWidth, _config.infillOverlap, (bridge > -1) ? bridge : fillAngle);
			}
			if (_config.enableCombing == COMBING_NOSKIN)
			{
				gcodeLayer.setCombBoundary(nullptr);
				gcodeLayer.setAlwaysRetract(true);
			}
			gcodeLayer.addPolygonsByOptimizer(skinPolygons, &_skinConfig);


			//After a layer part, make sure the nozzle is inside the comb boundary, so we do not retract on the perimeter.
			//一层的一部分后，确保喷嘴梳边界内，所以我们不收回的周长。
			if (!_config.spiralizeMode || static_cast<int>(layerNr) < _config.downSkinCount)
				gcodeLayer.moveInsideCombBoundary(_config.extrusionWidth * 2);
		}
		gcodeLayer.setCombBoundary(nullptr);
	}


	/**
	时间：2016年8月4日 11:34:45
	内容：添加填充的gocde
		参数：
		SliceLayerPart* part -->
		GCodePlanner& gcodeLayer -->
		int layerNr -->
		int extrusionWidth -->
		int fillAngle -->
	作者：TwoSilly
	*/
	void Processor::addInfillToGCode(SliceLayerPart* part, GCodePlanner& gcodeLayer, int layerNr, int extrusionWidth, int fillAngle)
	{
		Polygons infillPolygons;
		if (_config.sparseInfillLineDistance > 0)
		{
			switch (_config.infillPattern)
			{
			case INFILL_AUTOMATIC://这里出的错
				generateAutomaticInfill(
					part->sparseOutline, infillPolygons, extrusionWidth,
					_config.sparseInfillLineDistance,
					_config.infillOverlap, fillAngle);
				break;

			case INFILL_GRID:
				generateGridInfill(part->sparseOutline, infillPolygons,
					extrusionWidth,
					_config.sparseInfillLineDistance,
					_config.infillOverlap, fillAngle);
				break;

			case INFILL_LINES:
				generateLineInfill(part->sparseOutline, infillPolygons,
					extrusionWidth,
					_config.sparseInfillLineDistance,
					_config.infillOverlap, fillAngle);
				break;

			case INFILL_CONCENTRIC:
				generateConcentricInfill(
					part->sparseOutline, infillPolygons,
					_config.sparseInfillLineDistance);
				break;
			case INFILL_BEEGIVE:
				generateBrokenLineInfill(
					part->sparseOutline, 
					infillPolygons,
					extrusionWidth,
					_config.sparseInfillLineDistance * 2,
					_config.infillOverlap, 0
					);
				break;
			}
		}

		gcodeLayer.addPolygonsByOptimizer(infillPolygons, &_infillConfig);
	}

	void Processor::addSupportToGCode(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr)
	{
		if (!storage.support.generated)
			return;

		if (_config.supportExtruder > -1)
		{
			int prevExtruder = gcodeLayer.getExtruder();
			if (gcodeLayer.setExtruder(_config.supportExtruder))
				addWipeTower(storage, gcodeLayer, layerNr, prevExtruder);

			if (storage.oozeShield.size() > 0 && storage.volumes.size() == 1)
			{
				gcodeLayer.setAlwaysRetract(true);
				gcodeLayer.addPolygonsByOptimizer(storage.oozeShield[layerNr], &_skirtConfig);
				gcodeLayer.setAlwaysRetract(!_config.enableCombing);
			}
		}
		int32_t z = _config.initialLayerThickness + layerNr * _config.layerThickness;
		SupportPolyGenerator supportGenerator(storage.support, z);
		for(unsigned int volumeCnt = 0; volumeCnt < storage.volumes.size(); volumeCnt++)
		{
			SliceLayer* layer = &storage.volumes[volumeCnt].layers_[layerNr];
			for(unsigned int n=0; n<layer->parts.size(); n++)
				supportGenerator.polygons_ = supportGenerator.polygons_.difference(layer->parts[n].outline.offset(_config.supportXYDistance));
		}
		//合同和扩大支援多边形小部分除去最后的多边形平滑一点。
		supportGenerator.polygons_ = supportGenerator.polygons_.offset(-_config.extrusionWidth * 3);
		supportGenerator.polygons_ = supportGenerator.polygons_.offset(_config.extrusionWidth * 3);
		sendPolygonsToGui("support", layerNr, z, supportGenerator.polygons_);

		vector<Polygons> supportIslands = supportGenerator.polygons_.splitIntoParts();

		PathOrderOptimizer islandOrderOptimizer(_gcode.getPositionXY());
		for(unsigned int n=0; n<supportIslands.size(); n++)
		{
			islandOrderOptimizer.addPolygon(supportIslands[n][0]);
		}
		islandOrderOptimizer.optimize();

		for(unsigned int n=0; n<supportIslands.size(); n++)
		{
			Polygons& island = supportIslands[islandOrderOptimizer.polyOrder_[n]];

			Polygons supportLines;
			if (_config.supportLineDistance > 0)
			{
				switch(_config.supportType)
				{
				case SUPPORT_TYPE_GRID:
					if (_config.supportLineDistance > _config.extrusionWidth * 4)
					{
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance*2, _config.infillOverlap, 0);
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance*2, _config.infillOverlap, 90);
					}else{
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance, _config.infillOverlap, (layerNr & 1) ? 0 : 90);
					}
					break;
				case SUPPORT_TYPE_LINES:
					if (layerNr == 0)
					{
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance, _config.infillOverlap + 150, 0);
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance, _config.infillOverlap + 150, 90);
					}else{
						generateLineInfill(island, supportLines, _config.extrusionWidth, _config.supportLineDistance, _config.infillOverlap, 0);
					}
					break;
				}
			}

			gcodeLayer.forceRetract();
			if (_config.enableCombing)
				gcodeLayer.setCombBoundary(&island);
			if (_config.supportType == SUPPORT_TYPE_GRID)
				gcodeLayer.addPolygonsByOptimizer(island, &_supportConfig);
			gcodeLayer.addPolygonsByOptimizer(supportLines, &_supportConfig);
			gcodeLayer.setCombBoundary(nullptr);
		}
	}

	void Processor::addInsetToGCode(SliceLayerPart* part, GCodePlanner& gcodeLayer, int layerNr)
	{
		if (_config.insetCount > 0)
		{
			if (_config.spiralizeMode)
			{
				if (static_cast<int>(layerNr) >= _config.downSkinCount)
					_inset0Config.spiralize = true;
				if (static_cast<int>(layerNr) == _config.downSkinCount && part->insets.size() > 0)
					gcodeLayer.addPolygonsByOptimizer(part->insets[0], &_insetXConfig);
			}
			for(int insetNr=part->insets.size()-1; insetNr>-1; insetNr--)
			{
				if (insetNr == 0)
					gcodeLayer.addPolygonsByOptimizer(part->insets[insetNr], &_inset0Config);
				else
					gcodeLayer.addPolygonsByOptimizer(part->insets[insetNr], &_insetXConfig);
			}
		}
	}

	void Processor::addWipeTower(SliceDataStorage& storage, GCodePlanner& gcodeLayer, int layerNr, int prevExtruder)
	{
		if (_config.wipeTowerSize < 1)
			return;
		//如果我们改变了挤出机，打印该喷嘴的擦拭/首相塔；
		gcodeLayer.addPolygonsByOptimizer(storage.wipeTower, &_supportConfig);
		Polygons fillPolygons;
		generateLineInfill(storage.wipeTower, fillPolygons, _config.extrusionWidth, _config.extrusionWidth, _config.infillOverlap, 45 + 90 * (layerNr % 2));
		gcodeLayer.addPolygonsByOptimizer(fillPolygons, &_supportConfig);

		//确保我们在擦塔上擦拭旧的挤出机。
		gcodeLayer.addTravel(storage.wipePoint - _config.extruderOffset[prevExtruder].p() + _config.extruderOffset[gcodeLayer.getExtruder()].p());
	}

};
