#include "Processor.h"
#include "multiVolumes.h"

namespace Engine {

	
	/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ�gui�����ӣ���gui����������������
			ע�����������̵ģ��Ҵ���������
		������
			int portNr --> �˿ں�

	���ߣ�TwoSilly
	*/
	void Processor::guiConnect(int portNr)
	{
		_guiSocket.connectTo("127.0.0.1", portNr);
	}

	/**
	ʱ�䣺2016��10��31��10:45:34
	���ݣ������׽��֣����;��θ�GUI
		������
			const char* name --> ��ǰ���ε����֣�֧�ţ����ȣ�
			int layerNr --> �����
			int32_t z --> z���С
			Polygons& polygons --> ��������

	���ߣ�TwoSilly
	*/
	void Processor::sendPolygonsToGui(const char* name, int layerNr, int32_t z, Polygons& polygons)
	{
		//Engine::logError("���;�����Ϣ���ȴ�gui�������ݣ�\n");
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
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ�����Ŀ���ļ�
		������
			const char* filename -->

	���ߣ�TwoSilly
	*/
	bool Processor::setTargetFile(const char* filename)
	{
		_gcode.setFilename(filename);
		if (_gcode.isOpened())
			_gcode.writeComment("Generated with Engine %s", VERSION);
		return _gcode.isOpened();
	}

	/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ������ļ�
		������
			const std::vector<std::string> &files -->

	���ߣ�TwoSilly
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
		


		Engine::logProgress("process", 1, 1);//.����һ���ļ��ѱ���ȫ�����ͼ���û�����
		Engine::log("Total time elapsed %5.2fs.\n", timeKeeperTotal.restart());
		//�Ҵ��������Щ,Ȼ���ҷ����ˣ������ⶫ��ò�����Ÿ���
		_guiSocket.sendNr(GUI_CMD_FINISH_OBJECT);

		return true;
	}

	
	/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ������ļ����������������Ƶ����ŵ��õĺ���
		������

	���ߣ�TwoSilly
	*/
	void Processor::finalize()
	{
		if (!_gcode.isOpened())
			return;
		_gcode.finalize(_maxObjectHeight, _config.moveSpeed, _config.endCode.c_str());
	}

	/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ�Ԥ�ȵļƻ�
		������

	���ߣ�TwoSilly
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
	ʱ�䣺2016��10��28��16:43:52
	���ݣ�׼��ģ�ͣ���gui����ģ��
	���ߣ�TwoSilly
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
				Engine::logError("����������Ϣ���ȴ�gui�������ݣ�\n");
				_guiSocket.sendNr(GUI_CMD_REQUEST_MESH);//���﷢�͸�gui��Ϳ�ס�ˡ���

				int32_t vertexCount = _guiSocket.recvNr();
				Engine::logError("���յ�����%d \n",vertexCount);//������û�ߵ�����ǵ�
				//Engine::logError("���Ұ:%d\n",vertexCount);
				int pNr = 0;
				Engine::log("Reading mesh from socket with %i vertexes\n", vertexCount);
				Point3 v[3];
				/*
				ʱ�䣺2016��9��27��16:23:29
				���ݣ�����Ϊ�˵õ�gui���������ֵ�����ֵ�����gui����ʱ��gui���ģ��ת��Ϊ�㴫��engine 
				���ߣ�TwoSilly
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
					if(test == nullptr) { // error while reading occurred��ȡ����ʱ����
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
				
				//��������������������൱һ�㣬����ֻ���ڵ���ʱ��������
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
			ʱ�䣺2016��12��16��14:28:35
			���ݣ������п����Ż��ĵط���
					��û�е׷�ʱ���ﲻ��Ҫ��ƫ������
					����ò��û��ʲôЧ�������ƣ������һ��Ǿ����������Űɣ�������û�òŶ�
			���ߣ�TwoSilly
			  */
			//������ƫ�Ƶ�ÿһ�㡣
				//���������������� �Ͳ���Ҫ����
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
			//�ܼƲ� ���Ҿ����ǳ�ʼ�������
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
				ʱ�䣺2016��12��16��15:29:22
				���ݣ�������ת��ӡ��ÿ���㣬������ת��������Ϊ���ų���ͬһ���������ֵ�Բ��
				���ߣ�
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

			int offsetAngle = static_cast<int>(tan(60.0*M_PI/180) * _config.layerThickness);//����������������һ��60�Ƚǡ�
			for(unsigned int layerNr=1; layerNr<totalLayers; layerNr++)
				storage.oozeShield[layerNr] = storage.oozeShield[layerNr].unionPolygons(storage.oozeShield[layerNr-1].offset(-offsetAngle));
			for(unsigned int layerNr=totalLayers-1; layerNr>0; layerNr--)
				storage.oozeShield[layerNr-1] = storage.oozeShield[layerNr-1].unionPolygons(storage.oozeShield[layerNr].offset(-offsetAngle));
		}
		Engine::log("Generated inset in %5.3fs\n", _timeKeeper.restart());

		for(unsigned int layerNr=0; layerNr<totalLayers; layerNr++)
		{
			if (!_config.spiralizeMode || static_cast<int>(layerNr) < _config.downSkinCount)    //ֻ�в�����/ downskin�����ĵ�X�㵱spiralizeѡ��
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
			ʱ�䣺2016��8��7�� 08:57:35
			���ݣ�������Ϊ�˲��Լӵģ�
				Engine::log( "startCode:",_config.startCode.c_str());
			���ߣ�TwoSilly
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
			//ͨ��Ӧ����С��ʱ����ٶ������������һ��
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
				//�������ķ����ڲ������[ fanfullonlayernr ]������0�����ٶ�0
				fanSpeed = fanSpeed * layerNr / _config.fanFullOnLayerNr;
			}
			_gcode.writeFanCommand(fanSpeed);

			gcodeLayer.writeGCode(_config.coolHeadLift > 0, static_cast<int>(layerNr) > 0 ? _config.layerThickness : _config.initialLayerThickness);
		}

		Engine::log("Wrote layers in %5.2fs.\n", _timeKeeper.restart());
		_gcode.tellFileSize();
		_gcode.writeFanCommand(0);

		//�����Ǵ�ӡ�������ʱ���洢����ĸ߶ȣ���Ϊ������Ҫ���ƶ�����һ��λ��ʱ������ǵ�ÿһ����
		_maxObjectHeight = (std::max)(_maxObjectHeight, storage.modelSize.z - _config.objectSink);
	}

		/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ���Ӳ�������Gocde����һ����һ�����������Gocde���һ����
		������
		SliceDataStorage& storage -->
		GCodePlanner& gcodeLayer -->
		int volumeIdx -->
		int layerNr -->
	���ߣ�TwoSilly
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

			// Add either infill or perimeter first depending on option��������ܱ�����ȡ����ѡ��
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
			//һ���һ���ֺ�ȷ��������߽��ڣ��������ǲ��ջص��ܳ���
			if (!_config.spiralizeMode || static_cast<int>(layerNr) < _config.downSkinCount)
				gcodeLayer.moveInsideCombBoundary(_config.extrusionWidth * 2);
		}
		gcodeLayer.setCombBoundary(nullptr);
	}


	/**
	ʱ�䣺2016��8��4�� 11:34:45
	���ݣ��������gocde
		������
		SliceLayerPart* part -->
		GCodePlanner& gcodeLayer -->
		int layerNr -->
		int extrusionWidth -->
		int fillAngle -->
	���ߣ�TwoSilly
	*/
	void Processor::addInfillToGCode(SliceLayerPart* part, GCodePlanner& gcodeLayer, int layerNr, int extrusionWidth, int fillAngle)
	{
		Polygons infillPolygons;
		if (_config.sparseInfillLineDistance > 0)
		{
			switch (_config.infillPattern)
			{
			case INFILL_AUTOMATIC://������Ĵ�
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
		//��ͬ������֧Ԯ�����С���ֳ�ȥ���Ķ����ƽ��һ�㡣
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
		//������Ǹı��˼���������ӡ������Ĳ���/��������
		gcodeLayer.addPolygonsByOptimizer(storage.wipeTower, &_supportConfig);
		Polygons fillPolygons;
		generateLineInfill(storage.wipeTower, fillPolygons, _config.extrusionWidth, _config.extrusionWidth, _config.infillOverlap, 45 + 90 * (layerNr % 2));
		gcodeLayer.addPolygonsByOptimizer(fillPolygons, &_supportConfig);

		//ȷ�������ڲ����ϲ��þɵļ�������
		gcodeLayer.addTravel(storage.wipePoint - _config.extruderOffset[prevExtruder].p() + _config.extruderOffset[gcodeLayer.getExtruder()].p());
	}

};
