#ifndef DATATYPEDEFINE_H
#define DATATYPEDEFINE_H

#ifndef NULL
#define NULL 0
#endif
/*
#define  unsigned int //������������
#define weightidx_t unsigned int //Ȩ����������
#define infilefloat_t double //�����ļ�����������
#define computfloat_t double //����ʱʹ�õ���������
*/
//typedef unsigned int featureidx_t; //������������, ϵ���������ݴ�С������ֵ

typedef long long weightidx_t; //Ȩ����������
typedef double computfloat_t; //����ʱʹ�õ���������
//typedef double infilefloat_t; //�����ļ�����������
//typedef unsigned int infileclk_t;//�����ļ���clk,nonclk����
//typedef unsigned int insparsefileidx_t; //ϡ�������ļ���������������
//typedef unsigned int inselectidx_t; //select_index�õ���������

//�����ļ���ʽ����
typedef long long size_file_t;//���ݴ�С��������
typedef double infile_clk_t;//clk, nonclk����
typedef unsigned int infile_binfeanum_t;//bin feature number����
typedef unsigned int infile_binidx_t;//bin feature index ����
typedef unsigned int infile_multifeanum_t;// multiple feature number����
typedef unsigned int infile_multiidx_t;// multiple feature index����
typedef double infile_multivaluefloat_t;// multiple feature value ����
//2012-01-18��ӣ���ϡ������
typedef unsigned int infile_densefeanum_t;//��ϡ��������������
typedef double infile_densefeavalue_t;//��ϡ����������
//ע��,infile_densefeanum_t��Ҫ��endlinesign_t����ͬ��ʵ�����ͣ������ж��Ƿ���densefeanum�ֶ�
typedef unsigned int endlinesign_t;//ÿ�н���������
#define ENDLINESIGN ((unsigned int)(-1))
//2013-03-27���,���ܾ�������
typedef double densematrix_comput_t;//���ܾ�����������

//��ȡ�ļ����
//��ȡ�ı��ļ�һ�е���󳤶�
#define MAX_READ_FILE_LINE 1024000
//ģ���ļ���ʽ
#define WRITE_MODEL_DELIMITER "\t"
#define READ_INITIALMODEL_DELIMITER "\t \1"
//��ȡenable_file�Ĳ���
//update_enable_file �ڼ��п�ʼ�����ݣ�Ӧ��Ϊ1
#define UPDATE_ENABLE_FILE_STARTLINEID 1
//С��ENABLE_BINARY_BOUNDRY��ֵ��ʾ��ѡ�У����ڵ��ڵı�ʾѡ��
#define ENABLE_BINARY_BOUNDRY 0.01

//�ַ������
#define MAXSTRLEN 256
//�洢�ļ������ַ�����󳤶�
#define MAX_FILENAME_LEN 256
//��ʱ�ַ������ȣ��洢�߳���
#define TEMP_STR_LEN 256

//Ĭ�ϲ���
//�����ļ���ʽ���
#define DEFAULT_XINPUT_NONCLKID 0
#define DEFAULT_XINPUT_CLKID sizeof(infile_clk_t)
#define DEFAULT_XINPUT_FEATURESTARTID 2*sizeof(infile_clk_t)
//2013-04-26 ���ݻ��ֲ���
#define DEFAULT_DATADIVIDE_INSGROUPLEN 10
#define DEFAULT_DATADIVIDE_BATCHGROUPNUM 100000
//Ĭ�����ģ���ļ���
#define DEFAULT_MODEL_FILENAME "model"
//�Ż����
#define DEFAULT_ITERNUMPERSAVE	15
#define DEFAULT_LAMBDA 1
#define DEFAULT_BETA 1
#define DEFAULT_M	100
#define DEFAULT_MINIDECREASERATE 0.0001
#define DEFAULT_MAXITERNUM 1000
#define DEFAULT_FIRSTSTEPSIZE 1
#define DEFAULT_FIRSTSTEPMAXNORM 10
#define DEFAULT_STEP_MAXNORMMULTIPLIER 0
#define DEFAULT_STEP_DECREASERATE 0.1
#define DEFAULT_STEP_MAXTRYTIME 10
#define DEFAULT_STEP_TOLERANCERATE 0.01
#define DEFAULT_LEN_LBFGS_QUEUE 10
//�ļ���ʽ
#define DEFAULT_INITMODEL_STARTLINEID 10 //��ʼģ���ļ��ڼ��п�ʼ������
#define DEFAULT_GUIDE_ITERNUM 0

//L-BFGS���
#define MAX_RHO 1e100
#define MIN_INVERSE_RHO 1e-100
//��С����
#define MIN_DENOMINATOR 1e-100
//��Сlog��
#define MIN_LOG_NUM 1e-100
//���exp��
#define MAX_EXP_NUM 300

//�߳�ÿ�εȴ�ʱ��
#define THREAD_WAIT_TIME_SEC 1 //��λΪ��,s
#define THREAD_WAIT_TIME_NSEC 1000000 //��λΪ����
#define THREAD_TASK_INITIAL_COUNT 1 //�߳����������ʼֵ
#define DEFAULT_THREADNUM 1 //Ĭ���߳���

//MPI���
#define MPI_MASTER_NODE 0 //MPI���ڵ����
#define MPI_TASKTYPE_EVAL 1 //�����ݶȺ͸�˹Ȼ
#define MPI_TASKTYPE_ABORT 0 //�˳�
typedef double mpi_vec_float_t; //����mpi�������ͽ��ջ�������
#define MPI_VEC_FLOAT MPI_DOUBLE //�������Ӧ

computfloat_t inline trauncate_exp_num(computfloat_t exp_num){
	if (exp_num > MAX_EXP_NUM){
		return MAX_EXP_NUM;
	}
	if (exp_num < - MAX_EXP_NUM){
		return - MAX_EXP_NUM;
	}
	return exp_num;
}

#endif
