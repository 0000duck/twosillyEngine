/**

�ļ�����Config.h

ʱ�䣺2016��7��4�� 09:29:54
���ݣ������ļ��࣬����һ�����õ�ȫ��ֵ���Ͷ�ȡ�����ļ��ķ���
���ߣ�TwoSilly
E-Mail:twosilly@foxmail.com
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <vector>

#include "../utils/floatpoint.h"

#include "../tool/settings.h"

//�����ļ����õı��
class _ConfigSettingIndex
{
public:
    const char* key; //��ֵ
    int* ptr;	//��ֵ

    _ConfigSettingIndex(const char* key, int* ptr) : key(key), ptr(ptr) {}
};

/**
ʱ�䣺2016��8��3�� 20:52:44
���ݣ������ļ��࣬����װ�����е������ļ�����Ҫ�Ĳ���
	���ݳ�Ա��
		 static Config *config; // �����������������ص���������
                                  
    //����
    int nozzleSize;  //����ֱ��
    int layerThickness;//���
    int initialLayerThickness;//��ʼ���
    int filamentDiameter;//ϸ˿ֱ��(�ҹ����ǲ��ϵ�ֱ��)
    int filamentFlow;//˿��������������
    int layer0extrusionWidth;//��0�������
    int extrusionWidth;//�������
    int insetCount;//��������������
    int downSkinCount;//��Ƥ��������������
    int upSkinCount;//��Ƥ������
    
    int skirtDistance;//��Ե����
    int skirtLineCount;//��Ե�м���
    int skirtMinLength;//��Ե����С����
    
    //Retraction settings
    //��������
    int retractionAmount;//������
    int retractionAmountPrime;//���ڵĻ���
    int retractionAmountExtruderSwitch;//����������������
    int retractionSpeed;//�����ٶ�
    int retractionMinimalDistance;//���˵���С����
    int minimalExtrusionBeforeRetraction;//��С��ѹǰ���ˣ�������
    int retractionZHop;//���˵�Z����ת

    //���� 
    int enableCombing; //ʹ����
    int enableOozeShield;//ʹ����������ӿǣ�����
    int wipeTowerSize;//�����߼ܴ�С
    int multiVolumeOverlap;//��ѡ����ص�

    //Speedup �ٶ�
    int initialSpeedupLayers;//��ʼ���ٲ�
    int initialLayerSpeed;//��ʼ���ٶ�
    int printSpeed;//��ӡ�ٶ�
    int inset0Speed;//����0���ٶ�
    int insetXSpeed;//����X���ٶ�
    int moveSpeed;//�ƶ��ٶ�
    int fanFullOnLayerNr;//������ȫ��������Χ

    //Infill settings
    //�������
    int sparseInfillLineDistance;//ϡ�ٵ�����ߵľ���
    int infillOverlap;//����ص�
    int infillSpeed;//����ٶ�
    int infillPattern;//�����ʽ
   
    INFILL_AUTOMATIC = 0,   //�Զ����    
    INFILL_GRID = 1,        //����    
    INFILL_LINES = 2,       //����    
    INFILL_CONCENTRIC = 3,  //ͬ��      
   /
    int skinSpeed;//��Ƥ�ٶ�
    int perimeterBeforeInfill;//�߽����

    //Support material
    //֧�Ų���
    int supportType;//֧�ŵ�����
    int supportAngle;//֧�ŵĽǶ�
    int supportEverywhere;//ȫ��֧��(�޴����ڵ�)
    int supportLineDistance;//֧���ߵľ��룡
    int supportXYDistance;//֧�ŵ�XY�����
    int supportZDistance;//֧�ŵ�Z�����
    int supportExtruder;//������

    //Cool settings
    //��ȴ����
    int minimalLayerTime;//��Ͳ��ʱ��
    int minimalFeedrate;//��С�ٶ�
    int coolHeadLift;//��ȴ��ӡͷ����
    int fanSpeedMin;//������С�ٶ�
    int fanSpeedMax;//��������ٶ�

    //Raft settings
    //ľ������
    int raftMargin;//ľ����Ե
    int raftLineSpacing;//ľ���������
    int raftBaseThickness;//����������
    int raftBaseLinewidth;//�����߿��
    int raftBaseSpeed;//��������ٶ�
    int raftInterfaceThickness;//�������
    int raftInterfaceLinewidth;//�������
    int raftInterfaceLineSpacing;//������
    int raftFanSpeed;//������ת��
    int raftSurfaceThickness;//������
    int raftSurfaceLinewidth;//�����߿��
    int raftSurfaceLineSpacing;//�����߼��
    int raftSurfaceLayers;//����ı����
    int raftSurfaceSpeed;//����ı����ٶ�
    int raftAirGap;//���������϶
    int raftAirGapLayer0;//���������϶��0

    FloatMatrix3x3 matrix;//����
    IntPoint objectPosition;//����λ��
    int objectSink;//�����

	ʱ�䣺2016��7��13�� 09:01:34
	���ݣ��Զ����ĵ�
		ֵ��
		 1 --> ƫ��Ϊģ����������С��ĺ͵�һ�롣z��Ϊ��С��
		 2 --> ��ƫ��
		 ��Ϊ1�Ҳ�����2 ���� --> x��y�᲻ƫ�� z��Ϊģ����С���zֵ
	ʹ�õص㣺�Ż�ģ��ʱ����ƫ����
	���ߣ�TwoSilly
	///
    int autoCenter;//�Զ�����

    int fixHorrible;//�޸����µ�
    int spiralizeMode;//������ģ��
    int simpleMode;//�򵥵�ģ��
    int gcodeFlavor;//Gocdeģ��

    IntPoint extruderOffset[MAX_EXTRUDERS];//��������ƫ�� MAX 16��
    std::string startCode;//��ʼGocde
    std::string endCode;//����Gocde
    std::string preSwitchExtruderCode;//Ԥ���ؼ���������
    std::string postSwitchExtruderCode;//�󿪹ؼ���������
���ߣ�TwoSilly
*/

class Config
{
private:
    std::vector<_ConfigSettingIndex> _index;
public:
    static Config *config; // �����������������ص���������
                                  
    //����
    int nozzleSize;  //����ֱ��
    int layerThickness;//���
    int initialLayerThickness;//��ʼ���
    int filamentDiameter;//ϸ˿ֱ��(�ҹ����ǲ��ϵ�ֱ��)
    int filamentFlow;//˿��������������
    int layer0extrusionWidth;//��0�������
    int extrusionWidth;//�������
    int insetCount;//��������������
    int downSkinCount;//��Ƥ��������������
    int upSkinCount;//��Ƥ������
    
    int skirtDistance;//��Ե����
    int skirtLineCount;//��Ե�м���
    int skirtMinLength;//��Ե����С����
    
    //Retraction settings
    //��������
    int retractionAmount;//������
    int retractionAmountPrime;//���ڵĻ���
    int retractionAmountExtruderSwitch;//����������������
    int retractionSpeed;//�����ٶ�
    int retractionMinimalDistance;//���˵���С����
    int minimalExtrusionBeforeRetraction;//��С��ѹǰ���ˣ�������
    int retractionZHop;//���˵�Z����ת

    //���� 
    int enableCombing; //ʹ����
    int enableOozeShield;//ʹ����������ӿǣ�����
    int wipeTowerSize;//�����߼ܴ�С
    int multiVolumeOverlap;//��ѡ����ص�

    //Speedup �ٶ�
    int initialSpeedupLayers;//��ʼ���ٲ�
    int initialLayerSpeed;//��ʼ���ٶ�
    int printSpeed;//��ӡ�ٶ�
    int inset0Speed;//����0���ٶ�
    int insetXSpeed;//����X���ٶ�
    int moveSpeed;//�ƶ��ٶ�
    int fanFullOnLayerNr;//������ȫ��������Χ

    //Infill settings
    //�������
    int sparseInfillLineDistance;//ϡ�ٵ�����ߵľ���
    int infillOverlap;//����ص�
    int infillSpeed;//����ٶ�
    int infillPattern;//�����ʽ
    /*
    INFILL_AUTOMATIC = 0,   //�Զ����    
    INFILL_GRID = 1,        //����    
    INFILL_LINES = 2,       //����    
    INFILL_CONCENTRIC = 3,  //ͬ��      
    */
    int skinSpeed;//��Ƥ�ٶ�
    int perimeterBeforeInfill;//�߽����

    //Support material
    //֧�Ų���
    int supportType;//֧�ŵ�����
    int supportAngle;//֧�ŵĽǶ�
    int supportEverywhere;//ȫ��֧��(�޴����ڵ�)
    int supportLineDistance;//֧���ߵľ��룡
    int supportXYDistance;//֧�ŵ�XY�����
    int supportZDistance;//֧�ŵ�Z�����
    int supportExtruder;//֧�ż�����

    //Cool settings
    //��ȴ����
    int minimalLayerTime;//��Ͳ��ʱ��
    int minimalFeedrate;//��С�ٶ�
    int coolHeadLift;//��ȴ��ӡͷ����
    int fanSpeedMin;//������С�ٶ�
    int fanSpeedMax;//��������ٶ�

    //Raft settings
    //ľ������
    int raftMargin;//ľ����Ե
    int raftLineSpacing;//ľ���������
    int raftBaseThickness;//����������
    int raftBaseLinewidth;//�����߿��
    int raftBaseSpeed;//��������ٶ�
    int raftInterfaceThickness;//�������
    int raftInterfaceLinewidth;//�������
    int raftInterfaceLineSpacing;//������
    int raftFanSpeed;//������ת��
    int raftSurfaceThickness;//������
    int raftSurfaceLinewidth;//�����߿��
    int raftSurfaceLineSpacing;//�����߼��
    int raftSurfaceLayers;//����ı����
    int raftSurfaceSpeed;//����ı����ٶ�
    int raftAirGap;//���������϶
    int raftAirGapLayer0;//���������϶��0

    FloatMatrix3x3 matrix;//����
    IntPoint objectPosition;//����λ��
    int objectSink;//�����
	/**
	ʱ�䣺2016��7��13�� 09:01:34
	���ݣ��Զ����ĵ�
		ֵ��
		 1 --> ƫ��Ϊģ����������С��ĺ͵�һ�롣z��Ϊ��С��
		 2 --> ��ƫ��
		 ��Ϊ1�Ҳ�����2 ���� --> x��y�᲻ƫ�� z��Ϊģ����С���zֵ
	ʹ�õص㣺�Ż�ģ��ʱ����ƫ����
	���ߣ�TwoSilly
	*/
    int autoCenter;//�Զ�����

    int fixHorrible;//�޸����µ�
    int spiralizeMode;//������ģ��
    int simpleMode;//�򵥵�ģ��
    int gcodeFlavor;//Gocdeģ��

    IntPoint extruderOffset[MAX_EXTRUDERS];//��������ƫ�� MAX 16��
    std::string startCode;//��ʼGocde
    std::string endCode;//����Gocde
    std::string preSwitchExtruderCode;//Ԥ���ؼ���������
    std::string postSwitchExtruderCode;//�󿪹ؼ���������

	//����ʱ�����
    int acceleration; //���ٶ�
    int max_acceleration[4]; //�����ٶ�
    int max_xy_jerk; //���xy�س�
    int max_z_jerk; //���z�س�
    int max_e_jerk; //���e�س�

    Config();

	/**
	ʱ�䣺2016��7��9�� 14:41:54
	���ݣ����ݼ�ֵ�����������ļ�
		������
		const char* key --> ��ֵ �������ļ����ݳ�Ա�����֣�
		const char* value --> ��ֵ �������ļ����ݳ�Ա����ֵ��
	���ߣ�TwoSilly
	*/
    bool setSetting(const char* key, const char* value);

    //��ȡ�����ļ�������.cfg��ʽ�ģ����������ļ���
    bool readSettings(void);
    
	/**
	ʱ�䣺2016��7��9�� 14:41:54
	���ݣ���ȡ�����������ļ�
		������
		const char* path --> �����ļ�������
	���ߣ�TwoSilly
	*/
    bool readSettings(const char* path);
    bool readSettings_ini(const char* path);
};
#endif //CONFIG_H
