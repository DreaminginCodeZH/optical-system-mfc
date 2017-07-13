// https://support.microsoft.com/en-us/help/148652
#include "core_afx.h"

//
// Created by Yuan Shuyun, Liang Mingshu, Chen Honghao on 17-7-5.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double* realRayTracing(int k, double l, double u, FILE* fp, double *obj_n, double *r, double *d);
double* realRayTracing_b(int k, double l, double u, FILE* fp, double *obj_n, double *r, double *d, double L);

double mathi(double n,double m,double i);
double mathl(double r,double ii, double uu);
double mathu(double u, double i, double ii);
double mathuf(double n,double m,double u,double r,double h);

char *core(int a, double W_max, double obj_l, double obj_h) {
    FILE *fp_struct = NULL, *fp_result = NULL, *fp_tracing = NULL;

    int k;
    int i, j;

    double K_W[5] = { 1, 0.85, 0.7, 0.5, 0.3 }; // ��ͬ�ӳ���
    double K_eta_1[5] = { 1, 0.85, 0.7, 0.5, 0.3 }; // �Ի�ʸ�������׾�ȡ��ϵ��
    double K_eta_2[3] = { 1, 0, -1 }; // �Ϲ��ߡ��ڶ�������ߡ� �¹���
    double K_eta_3[11] = { 1, 0.85, 0.7, 0.5, 0.3, 0, -0.3, -0.5, -0.7, -0.85, -1 }; // ������������׾�ȡ��ϵ��
    double obj_u, obj_w;
    double inc_h, inc_i, im_u, im_h, re_i; // ���������뾶������ǣ������
    double r[13], obj_n[14], obj_nc[14], obj_nf[14], d[12];//����ÿ��������ʰ뾶�����������ʣ�������
    double obj_lfirst; //�����һ�������ࣻ
    double obj_ufirst; //�����һ������﷽��б�ǣ�
    double im_lfirst; //�����һ�������ࣻ
    double im_ufirst; //�����һ���������б�ǣ�
    double beta; // ��������Ŵ���
    double stop_r, stop_l; // ��������뾶�� λ��
    double obj_lg=obj_l;
    double l;
    double im_lg; // �����˹����λ��
    double obj_lsecond; //����ڶ��������ࣻ
    double obj_usecond; //����ڶ�������﷽��б�ǣ�
    double im_lsecond; //����ڶ��������ࣻ
    double im_usecond; //����ڶ����������б�ǣ�
    double sph_l, ch_l, ch_y, ast_l, fieldcur_s, fieldcur_t, dist_y,coma_yt, sph_lc, sph_lf;
    // ������λ��ɫ�����ɫ���ɢ����ʸ���������糡�������䡢�������
    double lf, lc; // F���� C�����
    double *tem, *tem_b; // ���¹���
    double coma_yab, coma_yp, coma_ypf, coma_ypc;double obj_uf=0;
    double im_uf;
    double obj_lp=stop_l;
    double im_lp;
    double obj_up=0.01;
    double im_up;
    double focus;
    double obj_h_2;
    double im_h_2;
    double obj_hfirst=obj_h;
    double im_hfirst;


    // �����ļ��еĹ�ѧ�ṹ����
    if((fp_struct = fopen("struct.txt","r"))==NULL)
    {
        return "File open error!";
    }

    fscanf(fp_struct,"%d",&k);//��ȡ����

    for(i=0; i<k; i++) {
        fscanf(fp_struct,"%lf",&r[i]);//��ȡÿ����뾶
    }
    for(i=0; i<k+1; i++) {
        fscanf(fp_struct,"%lf",&obj_n[i]);//��ȡÿ�ֽ���������
    }
    for(i=0; i<k-1; i++) {
        fscanf(fp_struct,"%lf",&d[i]);//��ȡ������
    }
    fscanf(fp_struct,"%lf",&stop_l);//��ȡ����λ��
    fscanf(fp_struct,"%lf",&stop_r);//��ȡ�����뾶

    for(i=0; i<k+1; i++) {
        fscanf(fp_struct,"%lf",&obj_nf[i]);//��ȡÿ�ֽ��� F ��������
    }

    for(i=0; i<k+1; i++) {
        fscanf(fp_struct,"%lf",&obj_nc[i]);//��ȡÿ�ֽ���C ��������
    }

    fp_result = fopen("Aberration.txt", "w");
    fp_tracing = fopen("Tracing.txt", "w");


    //��ʼ��������

    if(a==1){
        W_max = W_max*3.141592654/180;
        obj_l = -10E10;
    }

    else if(a==0){
        obj_lg=obj_l;
        obj_h_2=obj_h;
    }

    else {
        return "Invalid input!";
    }



    //���㽹��
    inc_h=stop_r;
    for(i=0;i<k;i++)
    {
        im_uf=mathuf(obj_n[i],obj_n[i+1],obj_uf,r[i],inc_h);
        if(i<k-1)
        {
            inc_h=inc_h-d[i]*im_uf;
            obj_uf=im_uf;
        }
    }
    focus=stop_r/im_uf;
    fprintf(fp_result, "���ࣺ");
    fprintf(fp_result, "%lf\n---\n",focus);

    fprintf(fp_tracing, "���ࣺ");
    fprintf(fp_tracing, "%lf\n---\n",focus);

    //�����ͫ��
    for(i=0;i<k;i++)
    {
        inc_i=(obj_lp-r[i])*obj_up/r[i];
        re_i=mathi(obj_n[i],obj_n[i+1],inc_i);
        im_up=mathu(obj_up,inc_i,re_i);
        im_lp=mathl(r[i],re_i,im_up);
        beta=beta*obj_n[i]*im_lp/(obj_n[i+1]*obj_lp);
        obj_lp=im_lp-d[i];
        obj_up=im_up;
    }
    fprintf(fp_result, "��ͫ�ࣺ");
    fprintf(fp_result, "%lf\n---\n", im_lp);

    fprintf(fp_tracing, "��ͫ�ࣺ");
    fprintf(fp_tracing, "%lf\n---\n", im_lp);


    // �����һ���������λ��

    beta=1;
    obj_hfirst=obj_h;
    //�������
    if(obj_l != -10E10)
    {
        obj_lfirst=obj_l;
        obj_ufirst = stop_r / sqrt(pow(stop_r,2) + pow((stop_l-obj_lfirst), 2));
        for(i=0;i<k;i++)
        {
            inc_i=(obj_lfirst-r[i]) * obj_ufirst / r[i];
            re_i = mathi(obj_n[i],obj_n[i+1],inc_i);
            im_ufirst = mathu(obj_ufirst,inc_i,re_i);
            im_lfirst = mathl(r[i],re_i,im_ufirst);
            beta=beta*obj_n[i]*im_lfirst/(obj_n[i+1]*obj_lfirst);
            obj_lfirst=im_lfirst-d[i];
            obj_ufirst = im_ufirst;
        }
        im_h=obj_hfirst*beta;

        // ���ݵ�д��

        fprintf(fp_result, "��һ����������: ");
        fprintf(fp_result, "%lf\n---\n", im_lfirst);
        fprintf(fp_result, "�������: ");
        fprintf(fp_result, "%lf\n---\n", im_h);

        fprintf(fp_tracing, "��һ����������: ");
        fprintf(fp_tracing, "%lf\n---\n", im_lfirst);
        fprintf(fp_tracing, "�������: ");
        fprintf(fp_tracing, "%lf\n---\n", im_h);
    } else
    {
        obj_ufirst=0;
        inc_h=stop_r;
        inc_i= inc_h/r[0];
        for(i=0;i<k;i++)
        {
            if(i!=0)
            {
                inc_i=(obj_lfirst-r[i]) * obj_ufirst / r[i];
            }
            re_i=mathi(obj_n[i],obj_n[i+1],inc_i);
            im_ufirst = mathu(obj_ufirst,inc_i,re_i);
            im_lfirst = mathl(r[i],re_i,im_ufirst);
            obj_lfirst=im_lfirst-d[i];
            obj_ufirst = im_ufirst;
        }
        im_h=focus*tan(W_max);

        // ���ݵ�д��
        fprintf(fp_result, "��һ����������: ");
        fprintf(fp_result, "%lf\n---\n", im_lfirst);
        fprintf(fp_result, "�������: ");
        fprintf(fp_result, "%lf\n---\n", im_h);

        fprintf(fp_tracing, "��һ����������: ");
        fprintf(fp_tracing, "%lf\n---\n", im_lfirst);
        fprintf(fp_tracing, "�������: ");
        fprintf(fp_tracing, "%lf\n---\n", im_h);
    }

    // ����ڶ�����������λ��
    if(obj_l==-10E10)  //������Զ
    {
        obj_lsecond=stop_l;
        obj_usecond=sin(W_max) ;
        for(i=0;i<k;i++)
        {
            inc_i=(obj_lsecond-r[i]) * obj_usecond / r[i];
            re_i=mathi(obj_n[i], obj_n[i+1], inc_i);
            im_usecond=mathu(obj_usecond, inc_i, re_i);
            im_lsecond=mathl(r[i], re_i, im_usecond);
            obj_lsecond=im_lsecond-d[i];
            obj_usecond = im_usecond;
        }
    }
    else //�����޾�
    {
        obj_lsecond=stop_l;
        obj_usecond=sin(atan(obj_h / (stop_l-obj_l))) ;
        for(i=0;i<k;i++)
        {
            inc_i=(obj_lsecond-r[i]) * obj_usecond / r[i];
            re_i=mathi(obj_n[i], obj_n[i+1], inc_i);
            im_usecond=mathu(obj_usecond, inc_i, re_i);
            im_lsecond=mathl(r[i], re_i, im_usecond);
            obj_lsecond=im_lsecond-d[i];
            obj_usecond = im_usecond;
        }
    }

    fprintf(fp_result, "�ڶ�����������: ");
    fprintf(fp_result, "%lf\n---\n", im_lsecond);

    fprintf(fp_tracing, "�ڶ�����������: ");
    fprintf(fp_tracing, "%lf\n---\n", im_lsecond);

    //ʵ�ʹ���׷��

    //�������Զ
    if(a == 1) {
        fprintf(fp_tracing, "����׷��:\n");
        fprintf(fp_tracing, "d��:\n");
        for (j = 0; j < 5; j++) {
            fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            fprintf(fp_result, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            obj_h = K_eta_1[j] * stop_r;
            tem = realRayTracing(k, obj_h, 0, fp_tracing, obj_n, r, d);
            sph_l = tem[1]-im_lfirst;
            fprintf(fp_result, "���: ");
            fprintf(fp_result, "%lf\n---\n", sph_l);
        }
        fprintf(fp_tracing, "---\n");

        //λ��ɫ����׾�ѭ��
        for (j = 0; j < 5; j++) {
            fprintf(fp_result, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            obj_h = K_eta_1[j] * stop_r;
            fprintf(fp_tracing, "C��: \n");
            tem = realRayTracing(k, obj_h, 0, fp_tracing, obj_nc, r, d);
            sph_lc = tem[1];
            fprintf(fp_result, "C��λ��: ");
            fprintf(fp_result, "%lf\n", sph_lc);

            fprintf(fp_tracing, "F��: \n");
            tem=realRayTracing(k, obj_h, 0, fp_tracing, obj_nf, r, d);
            sph_lf=tem[1];

            fprintf(fp_tracing, "---\n");

            fprintf(fp_result, "F��λ��: ");
            fprintf(fp_result, "%lf\n", sph_lc);

            fprintf(fp_result, "λ��ɫ��: ");
            fprintf(fp_result, "%lf\n---\n", sph_lf-sph_lc);
        }

    } else {
        fprintf(fp_tracing, "�����������׷��:\n");
        for (j = 0; j < 5; j++) {
            fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            fprintf(fp_result, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            obj_u = atan(K_eta_1[j] * stop_r/obj_l);
            tem=realRayTracing(k, obj_l, obj_u, fp_tracing, obj_n, r, d);
            sph_l=tem[1]-im_lfirst;
            fprintf(fp_result, "���: ");
            fprintf(fp_result, "%lf\n---\n", sph_l);
        }

        // λ��ɫ��
        for (j = 0; j < 5; j++) {
            fprintf(fp_result, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_1[j]);
            obj_u = atan(K_eta_1[j] * stop_r/obj_l);
            fprintf(fp_tracing, "C��: \n");
            tem=realRayTracing(k, obj_l, obj_u, fp_tracing, obj_nc, r, d);
            sph_lc=tem[1];
            fprintf(fp_result, "C��λ��: ");
            fprintf(fp_result, "%lf\n", sph_lc);

            fprintf(fp_tracing, "F��: \n");
            tem=realRayTracing(k, obj_l, obj_u, fp_tracing, obj_nf, r, d);
            sph_lf=tem[1];

            fprintf(fp_tracing, "---\n");

            fprintf(fp_result, "F��λ��: ");
            fprintf(fp_result, "%lf\n", sph_lc);

            fprintf(fp_result, "λ��ɫ��: ");
            fprintf(fp_result, "%lf\n---\n", sph_lf-sph_lc);
        }
    }

    // ʵ���������׷��

    if(a==1){
        fprintf(fp_tracing, "����ƽ�й�׷��:\n");
        coma_yt = 0;
        for (i = 0; i < 5; i++) {
            coma_yab=0;
            fprintf(fp_tracing, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            for (j = 0; j < 3; j++) {
                fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_2[j]);
                obj_u = K_W[i] * W_max;
                l = stop_l + K_eta_2[j] * stop_r / tan(obj_u);

                //�����߻�ʸ���繲ͬ׷��
                if (K_eta_2[j] == 0) {
                    fprintf(fp_result, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
                    tem_b=realRayTracing_b(k, l, obj_u, fp_tracing, obj_n, r, d, 0);
                    coma_yp =(tem_b[1]-im_lfirst) * tem_b[0];

                    fprintf(fp_result, "ʵ�����: ");
                    fprintf(fp_result, "%lf\n", coma_yp);
                }

                else {
                    tem=realRayTracing(k, l, obj_u, fp_tracing, obj_n, r, d);
                    coma_yab = coma_yab+(tem[1]-im_lfirst) * tem[0];
                }
            }

            fprintf(fp_tracing,"---\n");

            coma_yt = coma_yab/2 - coma_yp;
            ast_l = tem_b[2] - tem_b[3];
            dist_y = -(coma_yp + im_h*(tan(K_W[i]*W_max)/tan(W_max)));

            fprintf(fp_result, "�������: ");
            fprintf(fp_result, "%lf\n", coma_yt);

            fprintf(fp_result, "���糡��: ");
            fprintf(fp_result, "%lf\n", tem_b[2]-im_lfirst);

            fprintf(fp_result, "��ʸ����: ");
            fprintf(fp_result, "%lf\n", tem_b[3]-im_lfirst);

            fprintf(fp_result, "��ɢ: ");
            fprintf(fp_result, "%lf\n", ast_l);

            fprintf(fp_result, "����: ");
            fprintf(fp_result, "%lfE-04\n---\n", dist_y*10000);
        }

        //����ɫ��
        for (i = 0; i < 5; i++) {
            fprintf(fp_result, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            fprintf(fp_tracing, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            obj_u = K_W[i] * W_max;
            l = stop_l;
            fprintf(fp_tracing, "C��:\n");
            tem=realRayTracing(k, l, obj_u, fp_tracing, obj_nc, r, d);
            coma_ypc =(tem[1]-im_lfirst) * tem[0];

            fprintf(fp_result, "C�����:");
            fprintf(fp_result, "%lf\n", coma_ypc);

            fprintf(fp_tracing, "F��:\n");
            tem=realRayTracing(k, l, obj_u, fp_tracing, obj_nf, r, d);
            coma_ypf =(tem[1]-im_lfirst) * tem[0];

            fprintf(fp_tracing,"---\n");

            fprintf(fp_result, "F�����:");
            fprintf(fp_result, "%lf\n", coma_ypf);
            fprintf(fp_result, "����ɫ�");
            fprintf(fp_result, "%lf\n---\n", coma_ypf-coma_ypc);
        }
        //�������޾���

    } else {
        fprintf(fp_tracing, "�����������׷��:\n");

        for (i = 0; i < 5; i++) {
            coma_yt=0;
            coma_yab=0;
            fprintf(fp_result, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            fprintf(fp_tracing, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            for (j = 0; j < 11; j++) {
                fprintf(fp_tracing, "�׾�ȡ��ϵ�� %.3lf:\n", K_eta_3[j]);
                obj_u = atan((K_W[i] * obj_h_2 - K_eta_3[j] * stop_r) / (stop_l - obj_l));
                l = stop_l + K_eta_3[j] * stop_r / tan(obj_u);

                if (K_eta_3[j] == 0) {
                    tem_b=realRayTracing_b(k, l, obj_u, fp_tracing, obj_n, r, d, obj_l);
                    coma_yp = (im_lfirst - tem_b[1]) * tem_b[0];

                    fprintf(fp_result, "ʵ�����: ");
                    fprintf(fp_result, "%lf\n", coma_yp);
                } else if (K_eta_3[j] == 1||K_eta_3[j] == -1){

                    tem=realRayTracing(k, l, obj_u, fp_tracing, obj_n, r, d);
                    coma_yab = coma_yab+(im_lfirst - tem[1]) * tem[0];

                } else tem = realRayTracing(k, l, obj_u, fp_tracing, obj_n, r, d);
            }

            fprintf(fp_tracing,"---\n");

            coma_yt = coma_yab/2 - coma_yp;
            ast_l = tem_b[2] - tem_b[3];
            dist_y = -(coma_yp + im_h*K_W[i]);

            fprintf(fp_result, "�������: ");
            fprintf(fp_result, "%lf\n", coma_yt);

            fprintf(fp_result, "���糡��: ");
            fprintf(fp_result, "%lf\n", tem_b[2]-im_lfirst);

            fprintf(fp_result, "��ʸ����: ");
            fprintf(fp_result, "%lf\n", tem_b[3]-im_lfirst);

            fprintf(fp_result, "��ɢ: ");
            fprintf(fp_result, "%lf\n", ast_l);

            fprintf(fp_result, "����: ");
            fprintf(fp_result, "%lfE-04\n---\n", dist_y*10000);
        }

        //����ɫ��
        for (i = 0; i < 5; i++) {
            coma_yt=0;
            fprintf(fp_result, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            fprintf(fp_tracing, "�ӳ�ȡ��ϵ�� %.3lf:\n", K_W[i]);
            obj_u = atan((K_W[i] * obj_h_2) / (stop_l - obj_l));
            l = stop_l;

            fprintf(fp_tracing, "C�⣺\n");
            tem_b=realRayTracing(k, l, obj_u, fp_tracing, obj_nc, r, d);
            coma_ypc = (im_lfirst - tem_b[1]) * tem_b[0];
            fprintf(fp_result, "C����ߣ� ");
            fprintf(fp_result, "%lf\n", coma_ypc);

            fprintf(fp_tracing, "F�⣺\n");
            tem_b=realRayTracing(k, l, obj_u, fp_tracing, obj_nf, r, d);
            coma_ypf = (im_lfirst - tem_b[1]) * tem_b[0];

            fprintf(fp_tracing, "---\n");

            fprintf(fp_result, "F����ߣ� ");
            fprintf(fp_result, "%lf\n", coma_ypf);
            fprintf(fp_result, "����ɫ� ");
            fprintf(fp_result,"%lf\n---\n", coma_ypf-coma_ypc);

        }

    }

    return NULL;
}

double* realRayTracing_b(int k, double l, double u, FILE* fp, double *obj_n, double *r, double *d, double L) {
    int i;
    double obj_u[13], im_u[13], obj_l[13], im_l[13], obj_i[13], im_i[13], sin_obj_i[13], sin_im_i[13];
    double obj_h[13], obj_t[13], obj_s[13], im_t[13], im_s[13], PA[13], D[13], x[13], lt, ls;
    double *result_b;

    obj_l[0] = l;
    obj_u[0] = u;
    result_b=malloc(sizeof(double)*4);
    for (i = 0; i < k; i++) {
        sin_obj_i[i] = ((obj_l[i] - r[i]) / r[i]) * sin(obj_u[i]);

        sin_im_i[i] = (obj_n[i] / obj_n[i + 1] )* sin_obj_i[i];

        obj_i[i] = asin(sin_obj_i[i]);
        im_i[i] = asin(sin_im_i[i]);
        im_u[i] = obj_u[i] + obj_i[i] - im_i[i];
        im_l[i] = r[i] + r[i] * sin_im_i[i] / sin(im_u[i]);
        obj_h[i] = obj_l[i] * obj_u[i];
        PA[i] = im_i[i] * sin(im_u[i]) / cos((im_i[i] - im_u[i]) / 2);
        x[i]=PA[i]*PA[i]/(2*r[i]);
        obj_l[i + 1] = im_l[i] - d[i];
        obj_u[i + 1] = im_u[i];
    }

    if(L==0){
        im_t[0] = obj_n[1] * cos(im_i[0])*cos(im_i[0]) / ((obj_n[1] * cos(im_i[0]) - obj_n[0] * cos(obj_i[0])) / r[0]);
        im_s[0] = obj_n[1] / ((obj_n[1] * cos(im_i[0]) - obj_n[0] * cos(obj_i[0])) / r[0]);
        D[0] = (obj_h[0] - obj_h[1]) / sin(im_u[0]);
        obj_t[1] = im_t[0] - D[0];
        obj_s[1] = im_s[0] - D[0];
        for (i = 1; i < k; i++) {

            im_t[i] = obj_n[i + 1] * cos(im_i[i])*cos(im_i[i]) / ((obj_n[i + 1] * cos(im_i[i]) - obj_n[i] * cos(obj_i[i]))
                                                                  / r[i] + obj_n[i] * cos(obj_i[i])*cos(obj_i[i]) / obj_t[i]);
            im_s[i] = obj_n[i + 1] / ((obj_n[i + 1] * cos(im_i[i]) - obj_n[i] * cos(obj_i[i])) / r[i]
                                      + obj_n[i] / obj_s[i]);
            D[i] = (d[i]-x[i]+x[i+1])/cos(im_u[i]);
            obj_t[i + 1] = im_t[i] - D[i];
            obj_s[i + 1] = im_s[i] - D[i];
        }
    }

    else{
        obj_t[0]=(L-x[0])/cos(obj_u[0]);
        obj_s[0]=(L-x[0])/cos(obj_u[0]);
        for (i = 0; i < k; i++) {

            im_t[i] = obj_n[i + 1] * cos(im_i[i])*cos(im_i[i]) / ((obj_n[i + 1] * cos(im_i[i]) - obj_n[i] * cos(obj_i[i]))
                                                                  / r[i] + obj_n[i] * cos(obj_i[i])*cos(obj_i[i]) / obj_t[i]);
            im_s[i] = obj_n[i + 1] / ((obj_n[i + 1] * cos(im_i[i]) - obj_n[i] * cos(obj_i[i])) / r[i]
                                      + obj_n[i] / obj_s[i]);
            D[i] = (d[i]-x[i]+x[i+1])/cos(im_u[i]);
            obj_t[i + 1] = im_t[i] - D[i];
            obj_s[i + 1] = im_s[i] - D[i];
        }
    }
    lt=im_t[k-1]*cos(im_u[k-1])+x[k-1];
    ls=im_s[k-1]*cos(im_u[k-1])+x[k-1];
    //���ݵ�д��
    fprintf(fp, "��Ϊ������\n");
    fprintf(fp, "���: "); // ���������
    fprintf(fp, "%lf\n", im_l[k - 1]);

    fprintf(fp, "���: "); // �������ӳ���
    fprintf(fp, "%lf\n---\n", im_u[k - 1]);

    result_b[0] = tan(im_u[k - 1]);
    result_b[1] = im_l[k - 1];
    result_b[2] = lt;
    result_b[3] = ls;

    return result_b;

}

double* realRayTracing(int k, double l, double u, FILE* fp, double *obj_n, double *r, double *d) {
    int i;
    double obj_u[13], im_u[13], obj_l[13], im_l[13], obj_i[13], im_i[13], sin_obj_i[13], sin_im_i[13];
    double *result;

    result=malloc(sizeof(double)*2);
    if (u == 0) {
        sin_obj_i[0] = l / r[0];
        sin_im_i[0] = obj_n[0] / obj_n[1] * sin_obj_i[0];

        obj_i[0] = asin(sin_obj_i[0]);
        im_i[0] = asin(sin_im_i[0]);
        im_u[0] = obj_i[0] - im_i[0];
        im_l[0] = r[0] + r[0] * sin_im_i[0] / sin(im_u[0]);
        obj_l[1] = im_l[0] - d[0];
        obj_u[1] = im_u[0];


        for (i = 1; i < k; i++) {
            sin_obj_i[i] = (obj_l[i] - r[i]) / r[i] * sin(obj_u[i]);
            sin_im_i[i] = obj_n[i] / obj_n[i + 1] * sin_obj_i[i];

            obj_i[i] = asin(sin_obj_i[i]);
            im_i[i] = asin(sin_im_i[i]);
            im_u[i] = obj_u[i] + obj_i[i] - im_i[i];
            im_l[i] = r[i] + r[i] * sin_im_i[i] / sin(im_u[i]);
            obj_l[i + 1] = im_l[i] - d[i];
            obj_u[i + 1] = im_u[i];
        }
    }
    else {
        obj_l[0] = l;
        obj_u[0] = u;
        for (i = 0; i < k; i++) {
            sin_obj_i[i] = (obj_l[i] - r[i]) / r[i] * sin(obj_u[i]);
            sin_im_i[i] = obj_n[i] / obj_n[i + 1] * sin_obj_i[i];

            obj_i[i] = asin(sin_obj_i[i]);
            im_i[i] = asin(sin_im_i[i]);
            im_u[i] = obj_u[i] + obj_i[i] - im_i[i];
            im_l[i] = r[i] + r[i] * sin_im_i[i] / sin(im_u[i]);
            obj_l[i + 1] = im_l[i] - d[i];
            obj_u[i + 1] = im_u[i];
        }

    }

    // ���ݵ�д��
    fprintf(fp, "���: ");
    fprintf(fp, "%lf\n", im_l[k - 1]);

    fprintf(fp, "����б��: ");
    fprintf(fp, "%lf\n---\n", im_u[k-1]);

    result[0] = tan(im_u[k-1]);
    result[1] = im_l[k-1];

    return result;

}

// ��������ʱ
double mathi(double n, double m, double i){
    double result;

    result = n * i / m;

    return result;
}

// �������
double mathl(double r, double ii, double uu){
    double result;

    result = r + r * ii / uu;

    return result;
}

// �����񷽿׾���
double mathu(double u, double i, double ii) {
    double result;

    result = u + i - ii;

    return result;
}

double mathuf(double n,double m,double u,double r,double h){
    double result;

    result=(n*u*r+m*h-n*h)/(m*r);

    return result;
}
