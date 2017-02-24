/*
文件名：Beegive.cpp
时间：2016年12月30日16:24:34
内容：这是蜂窝创建类，我自己写的，估计会有很多问题，等测试
作者：TwoSilly
*/
#include "Beegive.h"
namespace Engine {

	/*
	时间：2016年12月30日11:00:45
	内容：产生正六边形 链表函数，通过传入 最小最大点生成 一个全 蜂窝填充的矩形
	参数：
	Point min_ --> 正六边形的起始点
	Point maX --> 正六边形的最大边界点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	bool CreateBeegiveList(Polygons &List, Point min_, Point maX, int side)
	{
		//vector<vector<Point>> List;
		//边长为大于0的数
		if (side <= 0) return false;
		if (&min_ == nullptr) return false;
		if (&maX == nullptr) return false;
		//int64_t

		//构建第一个多边形用于 偏移
		Polygon countList = CreateBeegive(min_, side);
		
		/*
		时间：2017年1月3日11:28:08
		内容：这是我做的一个bug验证没什么用
		作者：TwoSilly
		*/
		//Polygon countListTwo = CreateBeegiveTwo(min_, side);

		//int temp = countList.size();
		if (countList.size() < 6)
		{
			return false;
		}
		//一开始多边形所占的宽（行间隔）
		//double lineSpacing = min_.X + (abs(side * SIN60) * 2);
		//double SpacingX = maX.X - min_.X;

		//这里加1的原因是 为了只能多不能少
		//int PolygonCount = SpacingX + 1 / lineSpacing;
		double PolygonWidth = (abs(side * SIN60) * 2);
		//多边形计数,可以产生多少个 正六边形（X轴）

					//向上取整
		//int PolygonWidthCount = ceilf(((maX.X - min_.X) + 1) / PolygonWidth);
		int PolygonWidthCount = (((maX.X - min_.X) + 1) / PolygonWidth);
		//我打算偷懒了 正常应该用原点即最小点算出 正六边形的 正区间的 现在我用他的第2点 的y - 得五点的 y就行了
		/*
		double PolygonHight	= (abs(side * SIN30) * 2) + side
		double SpacingY = maX.Y - min_.Y;
		*/
		double PolygonHight = (abs(side * SIN30) * 2) + side;
		//int PolygonHightCount = ceilf(((maX.Y - min_.Y) + 1) / PolygonHight);
		int PolygonHightCount = (((maX.Y - min_.Y) + 1) / PolygonHight);



		//把第一个矩形压入 链表
		//List.push_back(countList);

		Polygons width;// = List;
		//width.push_back(countList);
		for (int i = 0; i < PolygonWidthCount + 1; i++)
		{
			//for (vector<Point>::iterator iter = countList.begin(); iter != countList.end(); iter++)
			//{
			//	 iter = + Point((abs(side * SIN60) * 2), 0);
			//}
			Polygon cut;// = width[i];
			for (int j = 0; j < countList.size(); j++)
			{
				Point  *p = new Point;
				*p = countList[j] + Point(PolygonWidth * i, 0);
				cut.add(*p);
			}
			width.add((cut));

			//Polygon cutTwo;// = width[i];
			//for (int j = 0; j < countListTwo.size(); j++)
			//{
			//	Point  *p = new Point;
			//	*p = countListTwo[j] + Point(PolygonWidth * i, 0);
			//	cutTwo.add(*p);
			//}
			//width.add(cutTwo);
		}

		for (int i = 0; i < PolygonHightCount; i++)
		{
			//for (vector<Point>::iterator iter = countList.begin(); iter != countList.end(); iter++)
			//{
			//	 iter = + Point((abs(side * SIN60) * 2), 0);
			//}
			// = List[i];
			for (int j = 0; j < width.size(); j++)
			{
				Polygon cut;
				for (int k = 0; k < width[j].size(); k++)
				{
					Point  *p = new Point;
					*p = width[j][k] + Point(0, (side + PolygonHight) * i);
					cut.add(*p);
				}
				List.add((cut));
			}

		}

		return true;
	}





	/*
	时间：2016年12月30日11:00:45
	内容：产生正六边形 函数，通过传入点为 第一点生成一个正六边形 边长为第二个参数（默认为6）
	参数：
	Point _min --> 正六边形的起始点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	Polygon CreateBeegive(Point pos, int side)
	{
		//用于存取正6边形 的链表 （一般每次存取6个值）
		//Polygons temp;
		
		Polygon countList;// (list); // = temp.newPoly();
		if (&pos == nullptr)
		{
			return countList;// .getPolygon();
		}
		//边长为大于0的数
		if (side <= 0) return countList;

		//第一个点

		countList.add(pos - Point(1,1));
		/*

		角度转弧度 π/180×角度
		弧度变角度 180/π×弧度
		*/
		//第二个点
		//double x = pos.X + abs(side * SIN60); //60度
		//double y = pos.Y + abs(side * SIN30); //30度
		countList.add(Point(pos.X + abs(side * SIN60), pos.Y + abs(side * SIN30)));

		//第三点
		//X = be.Beehive_[0].X + abs(side * SIN60) * 2;
		//Y = be.Beehive_[0].Y;
		countList.add(Point(pos.X + abs(side * SIN60) * 2, pos.Y));

		//第四个
		//be.Beehive_[3].X = be.Beehive_[0].X + abs(side * SIN60) * 2;
		//be.Beehive_[3].Y = be.Beehive_[0].Y - side;
		countList.add(Point(pos.X + abs(side * SIN60) * 2, pos.Y - side));

		//第五个
		//be.Beehive_[4].X = be.Beehive_[0].X + abs(side * SIN60);
		//be.Beehive_[4].Y = be.Beehive_[0].Y - side - abs(side * SIN30);
		countList.add(Point(pos.X + abs(side * SIN60), pos.Y - side - abs(side * SIN30)));

		//第六个
		//be.Beehive_[5].X = be.Beehive_[0].X;
		//be.Beehive_[5].Y = be.Beehive_[0].Y - side;
		countList.add(Point(pos.X, pos.Y - side));

		//这第七个点其实是初始点 我想实验点东西米有什么用的 记得要改掉
		//countList.add(pos);
		return countList;

	}


	/*
	时间：2017年1月3日11:10:35
	内容：产生正六边形 函数，通过传入点为 第一点生成一个正六边形 边长为第二个参数（默认为6）
	参数：
	Point _min --> 正六边形的起始点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	Polygon CreateBeegiveTwo(Point pos, int side)
	{
		//用于存取正6边形 的链表 （一般每次存取6个值）
		//Polygons temp;
		
		Polygon countList;// (list); // = temp.newPoly();
		if (&pos == nullptr)
		{
			return countList;// .getPolygon();
		}
		//边长为大于0的数
		if (side <= 0) return countList;

		//第一个点

		countList.add(pos - Point(1,1));
		/*

		角度转弧度 π/180×角度
		弧度变角度 180/π×弧度
		*/
		//第二个点
		//double x = pos.X + abs(side * SIN60); //60度
		//double y = pos.Y + abs(side * SIN30); //30度
		countList.add(Point(pos.X + abs(side * SIN60), pos.Y + abs(side * SIN30)));

		//第三点
		//X = be.Beehive_[0].X + abs(side * SIN60) * 2;
		//Y = be.Beehive_[0].Y;
		countList.add(Point((pos.X + abs(side * SIN60)), (pos.Y + abs(side * SIN30) + side)));

		//第四个
		//be.Beehive_[3].X = be.Beehive_[0].X + abs(side * SIN60) * 2;
		//be.Beehive_[3].Y = be.Beehive_[0].Y - side;
		countList.add(Point(pos.X, (pos.Y + abs(side * SIN30)* 2 + side)));

		//第五个
		//be.Beehive_[4].X = be.Beehive_[0].X + abs(side * SIN60);
		//be.Beehive_[4].Y = be.Beehive_[0].Y - side - abs(side * SIN30);
		countList.add(Point((pos.X - abs(side * SIN60)), (pos.Y + side + abs(side * SIN30))));

		//第六个
		//be.Beehive_[5].X = be.Beehive_[0].X;
		//be.Beehive_[5].Y = be.Beehive_[0].Y - side;
		countList.add(Point((pos.X - abs(side * SIN60)), (pos.Y + abs(side * SIN30))));

		//这第七个点其实是初始点 我想实验点东西米有什么用的 记得要改掉
		//countList.add(pos);
		return countList;

	}


	/*
	时间：2017年1月3日17:01:01
	内容：是的 ，我前面的算法有问题不完美，这个是改进版,左偏移
	参数：
	Point min_ --> 正六边形的起始点
	Point max_ --> 正六边形的开始点
	int side  --> 正六边形的边长
	int dir --> 向左还是向右偏移 0 向右 1向左
	作者：TwoSilly
	*/
	Polygon CreateBeegiveLineLeft(Point min_,Point max_, Point pos, int side)
	{
		//ClipperLib::Path list;
		Polygon countList;// (list); // = temp.newPoly();
		if (&min_ == nullptr || &pos == nullptr || min_ == pos)
		{
			return countList;// .getPolygon();
		}
		//边长为大于0的数
		if (side <= 0) return countList;

		//把初始点加进去
		countList.add(pos);
		while (true)
		{

			//Point next0 = CreatePoint(countList[countList.size() - 1], side,30);
			countList.add(CreatePoint(countList[countList.size() - 1], side, 150));
			countList.add(CreatePoint(countList[countList.size() - 1], side, 90));


			//当x 或者 y 大于 最大点的x  y 就证明已经出边界了！
			if ((countList[countList.size() - 1].X) <= (min_.X) ||
				(countList[countList.size() - 1].Y) >= (max_.Y)
				)
			{
				break;
			}
		}
		//补一个插脚
		countList.add(CreatePoint(countList[countList.size() - 1], side, 30));
		countList.add(CreatePoint(countList[countList.size() - 1], side, 330));
		while (true)
		{

			//Point next0 = CreatePoint(countList[countList.size() - 1], side,30);
			countList.add(CreatePoint(countList[countList.size() - 1], side, 270));
			countList.add(CreatePoint(countList[countList.size() - 1], side, 330));


			//当x 或者 y 大于 最大点的x  y 就证明已经出边界了！
			if (countList[countList.size() - 1].X >= max_.X ||
				countList[countList.size() - 1].Y <= min_.Y)
			{
				break;
			}
		}

		/*countList.add(CreatePoint(countList[countList.size() - 1], side, 270));
		countList.add(CreatePoint(countList[countList.size() - 1], side, 210));*/



		return countList;
	}

	/*
	时间：2017年1月3日17:01:01
	内容：是的 ，我前面的算法有问题不完美，这个是改进版
	参数：
	Point min_ --> 正六边形的起始点
	Point max_ --> 正六边形的结束点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	Polygon CreateBeegiveLineRight(Point min_, Point max_, int side)
	{
		//ClipperLib::Path list;
		Polygon countList;// (list); // = temp.newPoly();
		if (&min_ == nullptr || &max_ == nullptr || min_ == max_)
		{
			return countList;// .getPolygon();
		}
		//边长为大于0的数
		if (side <= 0) return countList;

			//把初始点加进去
			countList.add(min_);

		//这是一个无限循环
	
		
				while (true)
				{		

					//Point next0 = CreatePoint(countList[countList.size() - 1], side,30);
					countList.add(CreatePoint(countList[countList.size() - 1], side, 30));		
					countList.add(CreatePoint(countList[countList.size() - 1], side, 90));
			

					//当x 或者 y 大于 最大点的x  y 就证明已经出边界了！
					if (countList[countList.size() - 1].X >= max_.X || 
						countList[countList.size() - 1].Y >= max_.Y)
					{
						break; 
					}
				}
				//补一个插脚
				countList.add(CreatePoint(countList[countList.size() - 1], side, 30));
				countList.add(CreatePoint(countList[countList.size() - 1], side, 330));
				while (true)
				{

					//Point next0 = CreatePoint(countList[countList.size() - 1], side,30);
					countList.add(CreatePoint(countList[countList.size() - 1], side, 270));
					countList.add(CreatePoint(countList[countList.size() - 1], side, 210));


					//当x 或者 y 大于 最大点的x  y 就证明已经出边界了！
					if (countList[countList.size() - 1].X <= min_.X ||
						countList[countList.size() - 1].Y <= min_.Y)
					{
						break;
					}
				}

				countList.add(CreatePoint(countList[countList.size() - 1], side, 270));
				countList.add(CreatePoint(countList[countList.size() - 1], side, 210));	
				countList.add(CreatePoint(countList[countList.size() - 1], side, 150));

		return countList;
	
	}

	/*
	时间：2016年12月30日11:00:45
	内容：产生正六边形 链表函数，通过传入 最小最大点生成 一个全 蜂窝填充的矩形
	参数：
	Point min_ --> 正六边形的起始点
	Point maX --> 正六边形的最大边界点
	int side = 6 --> 正六边形的边长
	作者：TwoSilly
	*/
	bool CreateBeegiveLineList(Polygons &List_, Point min_, Point max_, int side)
	{
		//vector<vector<Point>> List;
		//边长为大于0的数
		if (side <= 0) return false;
		if (&min_ == nullptr) return false;
		if (&max_ == nullptr) return false;
		//Polygons List_Right;
		//Polygons List_Left;
		unsigned int width_R = (max_.X - min_.X + 1) / (abs(sin(AngleToRadian(60))) * side * 2 * 2);
		unsigned int height_R = (max_.Y - min_.Y + 1) /  ((abs(sin(AngleToRadian(30))) * side * 3 + side + side) * 2);

		unsigned int width_L = (max_.X - min_.X + 1) / ((abs(sin(AngleToRadian(60))) * side) * 2 * 2);
		unsigned int height_L = (max_.Y - min_.Y + 1) / ((abs(sin(AngleToRadian(30))) * side * 3 + side + side) * 2);
		//用于偏移的点
		Point pos = min_;
		/*
		往右偏
		*/
		////x轴

		for (unsigned int idx = 0; idx <= width_R; idx++)
		{
			//构建第一个多边形用于 偏移
			Polygon countList = CreateBeegiveLineRight(
												pos + Point(
															(abs(sin(AngleToRadian(60))) * side * 2) * idx * 2
															,0),

													max_, side);
			List_.add(countList.GetPoly());
		}

		////y轴
		for (unsigned int idx = 1; idx <= height_R; idx++)
		{
			//构建第一个多边形用于 偏移
			Polygon countList = CreateBeegiveLineRight(
				pos + Point(
				0
				, idx * ((((abs(sin(AngleToRadian(60))) * side) * 2) + side + side))*2),

				max_, side);
			List_.add(countList.GetPoly());
		}
		

		/*
		往左偏
		*/
		//x轴
		//pos = pos + Point((2 * (abs(sin(AngleToRadian(60)))) * side * 2), 0);
		for (unsigned int idx = 0; idx <= width_L + 1; idx++)
		{
			//构建第一个多边形用于 偏移
			Polygon countList = CreateBeegiveLineLeft(pos,max_,
				pos + Point(
				(abs(sin(AngleToRadian(60))) * side * 2) * idx * 2
				, 0),

				 side);
			List_.add(countList.GetPoly());
		}

		pos = List_[List_.size() - 1][0] + Point(2 * (abs(sin(AngleToRadian(60)))) * side * 2, 0);
		//y轴
		for (unsigned int idx = 0; idx <= height_L; idx++)
		{
			//构建第一个多边形用于 偏移
			Polygon countList = CreateBeegiveLineLeft(min_, max_,
				pos + Point(
				0
				, idx * ((((abs(sin(AngleToRadian(60))) * side) * 2) + side + side) * 2)),

				 side);
			List_.add(countList.GetPoly());
		}


	}
} //namespace Engine 