

�ر�
�Ӹ��Ĳ���
רע�������Ȱ�����
 Ŀ¼��ͼ ժҪ��ͼ ����
 ����RTPЭ���H.264��Ƶ����ϵͳ��ʵ��
��ǩ�� ��ý��rtph.264
2016-11-02 16:59 1246���Ķ� ����(2) �ղ� �ٱ�
 ���ࣺ ��ý�壨51��  
��Ȩ����������Ϊ�Ӹ��http://blog.csdn.net/caoshangpaԭ�����£�ת�������������
ʵ�ֵ�ԭ������RTPЭ���H.264��Ƶ����ϵͳ��ԭ��
������£�
��1��RTPЭ�����
��2��jrtplib���
��3��Qt����jrtplibʵ�ֵ������ಥ�͹㲥
��4��RTP ��Ч����(�غ�)���ͣ�RTP Payload Type
��5��H.264��H264����Ƶ�ļ�������
��6��H.264��ʽ����
��7��H.264��Ƶѹ����׼
����RTP Payload Format for H.264 Videoһ��Ҫ�ο��ĵ�rfc6184����Ϊrfc3984�Ѿ��������ˣ�rfc6184�������������Ӷ����Դ򿪡�
https://tools.ietf.org/html/rfc6184
https://datatracker.ietf.org/doc/rfc6184/?include_text=1
RTP����ΪH.264���������ֲ�ͬ�Ļ����ĸ��ؽṹ�����ն˿���ͨ��RTP���ص����ֽ���ʶ�����ǡ���һ���ֽ�����NALUͷ�ĸ�ʽ�����������ֶ���ָ���˴��������һ�ֽṹ������ֽڵĽṹ���£�
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
Type�������£�
0     û�ж���
1-23  NAL��Ԫ   ����NAL��Ԫ��.
24    STAP-A   ��һʱ�����ϰ�
25    STAP-B   ��һʱ�����ϰ�
26    MTAP16   ���ʱ�����ϰ�
27    MTAP24   ���ʱ�����ϰ�
28    FU-A     ��Ƭ�ĵ�Ԫ
29    FU-B     ��Ƭ�ĵ�Ԫ
30-31 û�ж���
���ֽڵ������ֶκ�H.264��NALUͷ�������ֶε������ǣ���Type��ֵΪ24~31��ʾ����һ���ر��ʽ��NAL��Ԫ����H.264�У�ֻȡ1~23����Ч��ֵ������ֱ�˵�������ָ��ؽṹ��

һ.Single NALU Packet����һNAL��Ԫģʽ��
��һ��RTP���ؽ������ֽں�һ��NALU������ɣ��ڱ����ж���С��1400�ֽڵ�NALU��������ִ��������������������ֽ������ֶκ�ԭʼ��H.264��NALUͷ�����ֶ���һ���ġ�Ҳ����˵�������������RTP�ĸ�����һ��������NALU��


��. Aggregation Packet����Ϸ��ģʽ��
��һ��RTP�з�װ���NALU�����ڽ�С��NALU���Բ������ִ���������Ӷ���ߴ���Ч�ʡ����������ɶ��NALU���һ��RTP�����ֱ���4����Ϸ�ʽ��STAP-A��STAP-B��MTAP16��MTAP24����ô�����RTP�������ֽ�����ֵ�ֱ���24��25��26��27������δ�漰����ģʽ��


��.Fragmentation Units����Ƭ���ģʽFUs��
һ��NALU��װ�ڶ��RTP�У�ÿ��RTP���������ֽڣ�����ʵ������FU indicator����������ԭ���ֽڵĽṹһ����������Ȼ�����ֽڣ���FU header��NALU���ص�һ������ɡ��ڱ����У����ڴ���1400�ֽڵ�NALU��������ַ������в������������������FU-A��FU-B������ֵ�ֱ���28��29��
FU-A��������ͼ��ʾ��
����ʹ�õ���FU-A���͡�

FU-B��������ͼ��ʾ��

��FU-A��ȣ�FU-B����һ��DON��decoding order number����DONʹ�õ��������ֽ���FU-Bֻ�����ڸ���ɨ����ģʽ�����������������档
FU indicator�ֽڽṹ������ʾ��
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
Type=28��29
FU header�ֽڽṹ������ʾ��
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
S��Start��: 1 bit�������ó�1����λָʾ��ƬNAL��Ԫ�Ŀ�ʼ��������FU���ز��Ƿ�ƬNAL��Ԫ�Ŀ�ʼ����λ��Ϊ0��
E��End��: 1 bit�������ó�1, ��λָʾ��ƬNAL��Ԫ�Ľ�������ʱ���ص�����ֽ�Ҳ�Ƿ�ƬNAL��Ԫ�����һ���ֽڡ�������FU���ز��Ƿ�ƬNAL��Ԫ�Ľ���,��λ��Ϊ0��
R��Reserved��: 1 bit������λ��������Ϊ0���ҽ����߱�����Ը�λ��
Type����NALUͷ�е�Typeֵ��ͬ

��.ʹ��socket�׽��ַ���RTP�����H264
��Ҫ����������ʾ��
[cpp] view plain copy
int main()  
{  
    OpenBitstreamFile("480320.264");  
    NALU_t *n;  
    char sendbuf[1500];  
  
    unsigned short seq_num =0;  
    int bytes=0;  
    InitWinsock(); //��ʼ���׽��ֿ�  
    int    sockfd;  
    struct sockaddr_in addr_in;  
    float framerate=25;  
    unsigned int timestamp_increse=0,ts_current=0;  
    timestamp_increse=(unsigned int)(90000.0 / framerate);  
  
    addr_in.sin_family=AF_INET;  
    addr_in.sin_port=htons(DEST_PORT);  
    addr_in.sin_addr.s_addr=inet_addr(DEST_IP);  
    sockfd=socket(AF_INET,SOCK_DGRAM,0);  
    connect(sockfd, (const sockaddr *)&addr_in, sizeof(sockaddr_in)) ;//����UDP�׽���  
    n = AllocNALU(8000000);//Ϊ�ṹ��nalu_t�����Աbuf����ռ䡣����ֵΪָ��nalu_t�洢�ռ��ָ��  
  
    while(!feof(bits))  
    {  
        GetAnnexbNALU(n);//ÿִ��һ�Σ��ļ���ָ��ָ�򱾴��ҵ���NALU��ĩβ����һ��λ�ü�Ϊ�¸�NALU����ʼ��0x000001  
        dump(n);//���NALU���Ⱥ�TYPE  
  
        memset(sendbuf,0,1500);//���sendbuf����ʱ�Ὣ�ϴε�ʱ�����գ������Ҫts_current�������ϴε�ʱ���ֵ  
  
        //rtp�̶���ͷ��Ϊ12�ֽ�,�þ佫sendbuf[0]�ĵ�ַ����rtp_hdr���Ժ��rtp_hdr��д�������ֱ��д��sendbuf��  
        rtp_hdr =(RTP_FIXED_HEADER*)&sendbuf[0];  
  
        //����RTP HEADER  
        rtp_hdr->version = 2;   //�汾�ţ��˰汾�̶�Ϊ2  
        rtp_hdr->marker  = 0;   //��־λ���ɾ���Э��涨��ֵ��  
        rtp_hdr->payload = H264;//�������ͺţ�  
        rtp_hdr->ssrc    = htonl(10);//���ָ��Ϊ10�������ڱ�RTP�Ự��ȫ��Ψһ  
  
        //��һ��NALUС��1400�ֽڵ�ʱ�򣬲���һ����RTP������  
        if(n->len<=MAX_RTP_PKT_LENGTH)  
        {  
            //����rtp M λ��  
            rtp_hdr->marker=1;  
            rtp_hdr->seq_no = htons(seq_num ++); //���кţ�ÿ����һ��RTP����1  
            ts_current=ts_current+timestamp_increse;  
            rtp_hdr->timestamp=htonl(ts_current);  
            //����NALU HEADER,�������HEADER����sendbuf[12]  
            nalu_hdr =(NALU_HEADER*)&sendbuf[12]; //��sendbuf[12]�ĵ�ַ����nalu_hdr��֮���nalu_hdr��д��ͽ�д��sendbuf�У�  
            nalu_hdr->F=n->forbidden_bit;  
            nalu_hdr->NRI=n->nal_reference_idc>>5;//��Ч������n->nal_reference_idc�ĵ�6��7λ����Ҫ����5λ���ܽ���ֵ����nalu_hdr->NRI��  
            nalu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[13],n->buf+1,n->len-1);//ȥ��naluͷ��naluʣ������д��sendbuf[13]��ʼ���ַ�����  
            bytes=n->len + 12 ;  //���sendbuf�ĳ���,Ϊnalu�ĳ��ȣ�����NALUͷ����ȥ��ʼǰ׺������rtp_header�Ĺ̶�����12�ֽ�  
            if(n->nal_unit_type==1 || n->nal_unit_type==5)  
            {  
  
                send(sockfd,sendbuf,bytes,0);//����RTP��  
            }  
            else  
            {  
                send(sockfd,sendbuf,bytes,0);//����RTP��  
                  //�����6,7���͵İ�����Ӧ����ʱ��֮ǰ��ͣ�٣�ԭ��������  
                 continue;  
            }  
        }  
        else  
        {  
            int packetNum = n->len/MAX_RTP_PKT_LENGTH;  
            if (n->len%MAX_RTP_PKT_LENGTH != 0)  
                packetNum ++;  
  
            int lastPackSize = n->len - (packetNum-1)*MAX_RTP_PKT_LENGTH;  
            int packetIndex = 1 ;  
  
            ts_current=ts_current+timestamp_increse;  
            rtp_hdr->timestamp=htonl(ts_current);  
  
            //���͵�һ����FU��S=1��E=0��R=0  
            rtp_hdr->seq_no = htons(seq_num ++); //���кţ�ÿ����һ��RTP����1  
            //����rtp M λ��  
            rtp_hdr->marker=0;  
            //����FU INDICATOR,�������HEADER����sendbuf[12]  
            fu_ind =(FU_INDICATOR*)&sendbuf[12]; //��sendbuf[12]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;//ʹ��FU-A  
  
            //����FU HEADER,�������HEADER����sendbuf[13]  
            fu_hdr =(FU_HEADER*)&sendbuf[13];  
            fu_hdr->S=1;  
            fu_hdr->E=0;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[14],n->buf+1,MAX_RTP_PKT_LENGTH);//ȥ��NALUͷ  
            bytes=MAX_RTP_PKT_LENGTH+14;//���sendbuf�ĳ���,Ϊnalu�ĳ��ȣ���ȥ��ʼǰ׺��NALUͷ������rtp_header��fu_ind��fu_hdr�Ĺ̶�����14�ֽ�  
            send( sockfd, sendbuf, bytes, 0 );//����RTP��  
  
            //�����м��FU��S=0��E=0��R=0  
            for(packetIndex=2;packetIndex<packetNum;packetIndex++)  
            {  
                rtp_hdr->seq_no = htons(seq_num ++); //���кţ�ÿ����һ��RTP����1  
  
                //����rtp M λ��  
                rtp_hdr->marker=0;  
                //����FU INDICATOR,�������HEADER����sendbuf[12]  
                fu_ind =(FU_INDICATOR*)&sendbuf[12]; //��sendbuf[12]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
                fu_ind->F=n->forbidden_bit;  
                fu_ind->NRI=n->nal_reference_idc>>5;  
                fu_ind->TYPE=28;  
  
                //����FU HEADER,�������HEADER����sendbuf[13]  
                fu_hdr =(FU_HEADER*)&sendbuf[13];  
                fu_hdr->S=0;  
                fu_hdr->E=0;  
                fu_hdr->R=0;  
                fu_hdr->TYPE=n->nal_unit_type;  
  
                memcpy(&sendbuf[14],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);//ȥ����ʼǰ׺��naluʣ������д��sendbuf[14]��ʼ���ַ�����  
                bytes=MAX_RTP_PKT_LENGTH+14;//���sendbuf�ĳ���,Ϊnalu�ĳ��ȣ���ȥԭNALUͷ������rtp_header��fu_ind��fu_hdr�Ĺ̶�����14�ֽ�  
                send( sockfd, sendbuf, bytes, 0 );//����rtp��  
            }  
  
            //�������һ����FU��S=0��E=1��R=0  
            rtp_hdr->seq_no = htons(seq_num ++);  
            //����rtp M λ����ǰ����������һ����Ƭʱ��λ��1  
            rtp_hdr->marker=1;  
            //����FU INDICATOR,�������HEADER����sendbuf[12]  
            fu_ind =(FU_INDICATOR*)&sendbuf[12]; //��sendbuf[12]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;  
  
            //����FU HEADER,�������HEADER����sendbuf[13]  
            fu_hdr =(FU_HEADER*)&sendbuf[13];  
            fu_hdr->S=0;  
            fu_hdr->E=1;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[14],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,lastPackSize-1);//��nalu���ʣ���-1(ȥ����һ���ֽڵ�NALUͷ)�ֽ�����д��sendbuf[14]��ʼ���ַ�����  
            bytes=lastPackSize-1+14;//���sendbuf�ĳ���,Ϊʣ��nalu�ĳ��ȼ�1����rtp_header��FU_INDICATOR,FU_HEADER������ͷ��14�ֽ�  
            send( sockfd, sendbuf, bytes, 0 );//����rtp��  
        }  
  
       Sleep(40);  
    }  
  
    FreeNALU(n);  
#ifdef WIN32  
    WSACleanup();  
#endif // WIN32  
    return 0;  
}  

��.ʹ��jrtplib����RTP�����H264
ʹ��jrtplib�ĺô��ǣ�����Ҫ�Լ������RTPͷ�ˣ�����jrtplib�ڲ�ʵ����RTCPЭ�顣jrtplib�ļ����Բο���2����ʹ�÷������Բο���3����
��Ҫ����������ʾ��
[cpp] view plain copy
int main()  
{  
    //��ʼ��jrtplib  
    uint8_t destIP[]={127,0,0,1};  
    initialRTP(destIP);  
    //��264�ļ�  
    OpenBitstreamFile("480320.264");  
    NALU_t *n;  
    char sendbuf[1500];  
    int bytes=0;  
  
    n = AllocNALU(8000000);//Ϊ�ṹ��nalu_t�����Աbuf����ռ䡣����ֵΪָ��nalu_t�洢�ռ��ָ��  
  
    while(!feof(bits))  
    {  
        GetAnnexbNALU(n);//ÿִ��һ�Σ��ļ���ָ��ָ�򱾴��ҵ���NALU��ĩβ����һ��λ�ü�Ϊ�¸�NALU����ʼ��0x000001  
        dump(n);//���NALU���Ⱥ�TYPE  
  
        memset(sendbuf,0,1500);//���sendbuf  
  
        //��һ��NALUС��1400�ֽڵ�ʱ�򣬲���һ����RTP������  
        if(n->len<=MAX_RTP_PKT_LENGTH)  
        {  
  
            //����NALU HEADER,�������HEADER����sendbuf[0]  
            nalu_hdr =(NALU_HEADER*)&sendbuf[0]; //��sendbuf[0]�ĵ�ַ����nalu_hdr��֮���nalu_hdr��д��ͽ�д��sendbuf�У�  
            nalu_hdr->F=n->forbidden_bit;  
            nalu_hdr->NRI=n->nal_reference_idc>>5;//��Ч������n->nal_reference_idc�ĵ�6��7λ����Ҫ����5λ���ܽ���ֵ����nalu_hdr->NRI��  
            nalu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[1],n->buf+1,n->len-1);//ȥ��naluͷ��naluʣ������д��sendbuf[1]��ʼ���ַ�����  
            bytes=n->len;  
            if(n->nal_unit_type==1 || n->nal_unit_type==5)  
            {  
                //��������Mark��RTPͷ�е�Mλ  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,true,3600);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
            }  
            else  
            {  
                //ע��˴���ʹ��socekt���͵Ĳ�ͬ����ʱʱ���������  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,true,0);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
                  //�����6,7���͵İ�����Ӧ����ʱ��֮ǰ��ͣ�٣�ԭ��������  
                 continue;  
            }  
        }  
        else  
        {  
            int packetNum = n->len/MAX_RTP_PKT_LENGTH;  
            if (n->len%MAX_RTP_PKT_LENGTH != 0)  
                packetNum ++;  
  
            int lastPackSize = n->len - (packetNum-1)*MAX_RTP_PKT_LENGTH;  
            int packetIndex = 1 ;  
  
  
            //���͵�һ����FU��S=1��E=0��R=0  
            fu_ind =(FU_INDICATOR*)&sendbuf[0]; //��sendbuf[0]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;//ʹ��FU-A  
  
            //����FU HEADER,�������HEADER����sendbuf[1]  
            fu_hdr =(FU_HEADER*)&sendbuf[1];  
            fu_hdr->S=1;  
            fu_hdr->E=0;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[2],n->buf+1,MAX_RTP_PKT_LENGTH);//ȥ��NALUͷ  
            bytes=MAX_RTP_PKT_LENGTH+2;//���sendbuf�ĳ���,Ϊnalu�ĳ��ȣ���ȥ��ʼǰ׺��NALUͷ����fu_ind��fu_hdr�Ĺ̶�����2�ֽ�  
            int status = m_session.SendPacket((void *)sendbuf,bytes,96,false,0);  
            if (status < 0)  
            {  
                std::cout<<RTPGetErrorString(status)<<std::endl;  
            }  
  
            //�����м��FU��S=0��E=0��R=0  
            for(packetIndex=2;packetIndex<packetNum;packetIndex++)  
            {  
                //����FU INDICATOR,�������HEADER����sendbuf[0]  
                fu_ind =(FU_INDICATOR*)&sendbuf[0]; //��sendbuf[0]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
                fu_ind->F=n->forbidden_bit;  
                fu_ind->NRI=n->nal_reference_idc>>5;  
                fu_ind->TYPE=28;  
  
                //����FU HEADER,�������HEADER����sendbuf[1]  
                fu_hdr =(FU_HEADER*)&sendbuf[1];  
                fu_hdr->S=0;  
                fu_hdr->E=0;  
                fu_hdr->R=0;  
                fu_hdr->TYPE=n->nal_unit_type;  
  
                memcpy(&sendbuf[2],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);//ȥ����ʼǰ׺��naluʣ������д��sendbuf[24]��ʼ���ַ�����  
                bytes=MAX_RTP_PKT_LENGTH+2;//���sendbuf�ĳ���,Ϊnalu�ĳ��ȣ���ȥԭNALUͷ������fu_ind��fu_hdr�Ĺ̶�����2�ֽ�  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,false,0);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
            }  
  
            //����FU INDICATOR,�������HEADER����sendbuf[0]  
            fu_ind =(FU_INDICATOR*)&sendbuf[0]; //��sendbuf[0]�ĵ�ַ����fu_ind��֮���fu_ind��д��ͽ�д��sendbuf�У�  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;  
  
            //����FU HEADER,�������HEADER����sendbuf[1]  
            fu_hdr =(FU_HEADER*)&sendbuf[1];  
            fu_hdr->S=0;  
            fu_hdr->E=1;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[2],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,lastPackSize-1);//��nalu���ʣ���-1(ȥ����һ���ֽڵ�NALUͷ)�ֽ�����д��sendbuf[2]��ʼ���ַ�����  
            bytes=lastPackSize-1+2;//���sendbuf�ĳ���,Ϊʣ��nalu�ĳ��ȼ�1����FU_INDICATOR,FU_HEADER������ͷ��2�ֽ�  
            status = m_session.SendPacket((void *)sendbuf,bytes,96,true,3600);  
            if (status < 0)  
            {  
                std::cout<<RTPGetErrorString(status)<<std::endl;  
            }  
        }  
  
       Sleep(40);  
    }  
  
    FreeNALU(n);  
  
    m_session.BYEDestroy(RTPTime(10,0),0,0);  
#ifdef WIN32  
    WSACleanup();  
#endif // WIN32  
  
    return 0;  
}  

��.����
�������ַ����Ĳ���Ч����ͬ����VLC����MPlayer��������ǿ���Ƽ�MPlayer��Ч����VLC�ã�����w.sdp���뵽�������У�����������ȴ�״̬��Ȼ�����г�����RTP������������ʼ���š�
����ʹ����176144.264��480320.264����H.264�ļ�����һ���ֱ���Ϊ176*144��NALU�Ƚ�С�����ʱ����Ҫ�ֶΣ��ڶ����ֱ���480*320��NALU����1400���̴��ڷֶδ���������
w.sdp
[cpp] view plain copy
m=video 25000 RTP/AVP 96     
a=rtpmap:96 H264             
a=framerate:25               
c=IN IP4 127.0.0.1           
ע��˿ں�25000Ҫ�ͳ��������õ���ͬ��
����Ч��������ʾ��



��.С��
����ʱ�������Ҫע�����h264�Ĳ�����Ϊ90000HZ�����ʱ����ĵ�λΪ1(��)/90000����������ǰ��Ƶ֡��Ϊ25fps����ʱ����������˵����Ӧ��Ϊ3600�����֡��Ϊ30fps��������Ϊ3000���Դ����ơ�
����h264���������FU-A��ʽ˵����
1.��һ��FU-A����FU indicator��FӦ��Ϊ��ǰNALUͷ��F����NRIӦ��Ϊ��ǰNALUͷ��NRI��Type�����28����������FU-A����FU header���ɷ�����S = 1��E = 0��R = 0��Type�����NALUͷ�е�Type��
2.������N��FU-A����FU indicator�͵�һ������ȫһ���ģ�����������һ��������FU headerӦ��Ϊ��S = 0��E = 0��R = 0��Type����NALUͷ�е�Type��
3.���һ��FU-A��FU headerӦ��Ϊ��S = 0��E = 1��R = 0��Type����NALUͷ�е�Type��
����ܽ���ǣ�ͬһ��NALU�ְ����FU indicatorͷ����ȫһ�µģ�FU headerֻ��S�Լ�Eλ�����𣬷ֱ��ǿ�ʼ�ͽ��������ǵ�RTP�ְ������к�Ӧ�������ε����ģ��������ǵ�ʱ�������һ�£�����������ΪNALU��ȥ��1���ֽڵ�NALUͷ���ʣ�����ݵĲ�֣����ܹؼ����������ΪNALUͷ����ֳ���FU indicator��FU header�����Բ�����Ҫ1�ֽڵ�NALUͷ�ˡ�
����SPS�Լ�PPS������֡�Ĵ����Ҳ������ȷ�SPS���ٷ���PPS����ʹ��ͬ����ʱ��������߰�������ʱ��������ٻ���������͵���ʽ����ò�ƶ����ԣ�����������ô�����ˣ�������һ�£��������ʹ��vlc���в��ŵĻ���������sdp�ļ�������SPS�Լ�PPS�������Ϳ��Բ��÷��������ˡ�
ʹ��VLC����ʱ��sdp�ļ��еķְ�ģʽѡ�packetization-mode=1�������ļ�ĩβ����=fmtp:packetization-mode=1�����������⡣
����packetization-mode����ϸ˵�����£�
��packetization-mode��ֵΪ0ʱ�򲻴���ʱ, ����ʹ�õ�һNALU��Ԫģʽ��
��packetization-mode��ֵΪ1ʱ����ʹ�÷ǽ���(non-interleaved)���ģʽ��
��packetization-mode��ֵΪ2ʱ����ʹ�ý���(interleaved)���ģʽ��
ÿ�������ʽ�����NAL��Ԫ�����ܽ�(yes = ����, no = ������, ig = ����)
Type   Packet    Single NAL    Non-Interleaved    Interleaved
                 Unit Mode           Mode             Mode
-------------------------------------------------------------

0      undefined     ig               ig               ig
1-23   NAL unit     yes              yes               no
24     STAP-A        no              yes               no
25     STAP-B        no               no              yes
26     MTAP16        no               no              yes
27     MTAP24        no               no              yes
28     FU-A          no              yes              yes
29     FU-B          no               no              yes
30-31  undefined     ig               ig               ig

Դ�����ӣ���http://blog.csdn.net/caoshangpa/article/details/53009604������

��
2
 
��
0
 
 
��һƪRTP ��Ч����(�غ�)���ͣ�RTP Payload Type
��һƪH.264��Ƶѹ����׼
  ��������Ƽ�
? ʵ��RTPЭ���H.264��Ƶ����ϵͳ
? rtp c++ ������Ƶ����
? RTP������Ƶ����
? RTPЭ��: Internet����Ƶ����
? RTPЭ���H.264��Ƶ����
? ����JMF RTP������Ƶ����
? H.264��Ƶ��RTP���ظ�ʽ
? H264����RTPЭ���ʵ��
? ����DirectShow��MPEG-4��Ƶ����ϵͳ���о���ʵ��
? [ת��]H.264 RTP payload ��ʽ(��Ч�غ�)
��������
����ѧϰ֮������ͳ���ƶ� ����ѧϰ֮��ѧ���� ����ѧϰ֮͹�Ż� ����ѧϰ֮���� ��Ӧʽ����ȫ��̽�� ̽��Linux�����ߡ��豸������ģ�� ���ѧϰ������TensorFlowʵ�� ���ѧϰ֮������ԭ����ʵս���� ǰ�˿������߷�� TensorFlowʵս���ף��ְ��ֽ�����ͼ��ʶ��Ӧ��
�鿴����
2¥ arbotor 2016-12-06 01:42���� [�ظ�] ��л¥������˽�����и����������һ�£�����ֱ����¥���Ĺ���ֱ�����еģ����Ǳ��������Ǳ���F:\Qt RTP H264\testJrtplib\main.cpp:277: error: undefined reference to `_imp___ZN7jrtplib17RTPGetErrorStringB5cxx11Ei'������ʲôԭ���أ������˺þ�Ҳû�ҵ�ԭ��
1¥ �Ӹ�� 2016-11-11 10:55���� [�ظ�] [����] [�ٱ�]Դ�����ӣ�http://download.csdn.net/detail/caoshangpa/9679976
��������
�� �� ����
yjduoduo
�������ݣ�
�������

�ύ  
* �����û�����ֻ��������˹۵㣬������CSDN��վ�Ĺ۵������
��������
�����ҵĿռ�  
�Ӹ��
 
 1  3
���ʣ�284205��
���֣�6480
�ȼ��� 
��������3513��
ԭ����143ƪת�أ�123ƪ���ģ�0ƪ���ۣ�252��
��������

 ����
����ר��
	WebRTCѧϰ
���£�11ƪ
�Ķ���13050
���·���
Qt(86)
Python(1)
3D(4)
other(52)
���ģʽ(24)
JavaScript(1)
C/C++(14)
libevent(3)
ѧϰ�ִ�3Dͼ�α��(4)
C++11����ѧϰ(6)
PhotoShop(2)
��ý��(52)
���㿪Դ��Ŀ(3)
WebRTC(11)
���´浵
2017��06��(8)
2017��05��(6)
2017��04��(3)
2017��03��(8)
2017��01��(11)
չ��
�Ķ�����
JWPlayer 7����ȷʹ�÷�����Flash plugin failed to load�������(8850)
Qt����������ķ���(7569)
Qt C++����ȫ�ֱ��������ַ�ʽ(6842)
TCP��UDP���ݰ���С������(6290)
Qt����ͨ�Ž������ݲ������Ľ������(6038)
Qt���ذٶ����ߵ�ͼ(5086)
ʹ��Qt5.7.0 VS2015�汾���ɼ���XP�Ŀ�ִ�г���(4541)
ʹ��VLC������������RTSP��(4379)
Adobe Media Server 5��AMS���İ�װ��ʹ��(4145)
FFmpeg֧�ֵ���Ƶ����Ƶ������ʽ(3855)
��������
Qt���ذٶ����ߵ�ͼ(33)
Qt����FFmpeg���ű��� H.264��H264���ļ�(16)
ͨ��live555ʵ��H264 RTSPֱ����Windows�棩(13)
Qt����PCM��Ƶ���������ļ������ַ���(11)
Qt����jrtplibʵ�ֵ������ಥ�͹㲥(9)
Windows 7��Win7����MinGW+msys����ffmpeg��������H264����֧��(6)
Qtʵ�ֻ���G.729A��G729A������������(6)
ʹ��Qt5.7.0 VS2015�汾���ɼ���XP�Ŀ�ִ�г���(5)
VS2015����Qt5.7.0����֧��XP�ľ�̬��(5)
Qt������������������(5)
�Ƽ�����
* CSDN�ձ�20170706��������˿����Ա����Ϯ֮�á�
* ̽�ֺ��ѡ���в�ͬ���Լ���Ӧ��Web���
* ϸ˵���䣬Java �� Android �����߱����Խ�Ŀ�
* ���ѧϰ | ���򴫲�������ֱ�����
* ArcGIS ˮ�ķ���ʵս�̡̳�����������������ͳ��
* ÿ�ܼ��飺Android��Keras��ES6���������飩
��������
C++������ String���ʵ��
qq_35434315: ����
Qt����FFmpeg���ű��� H.264��H264���ļ�
С��_��: @qq_36685978:��ã���Ҳ�ǳ�����UI���棬û��ͼ������������ô����ģ�
Qt����FFmpeg���ű��� H.264��H264���ļ�
С��_��: @qq_36685978:��ã���Ҳ�Ǳ���ú�û��ͼ������������ô����ģ�
WebRTCѧϰ֮�ˣ��������첽ִ��
�������԰�: ͬ�ʣ�Ϊ��Ҫʵ����Ϣѭ������������ʵ�֣��رմ����ˣ�������ʵ��û����
JWPlayer 7����ȷʹ�÷�����Flash plugin failed to load�������
��С��: ���õ���xampp��Ϊ������ �����ǲ��Ų���flv����Ƶ ��ô��
Qt����FFmpeg���ű��� H.264��H264���ļ�
����Ŀ����: �����һ֡һ֡��������˵һ��gopһ��gop����������P֡�Ļ��ǲ���Ӧ�ú�I֡һ���������
Qt����FFmpeg���ű��� H.264��H264���ļ�
����Ŀ����: ʵʱ����264������������ӳټӿ��ٵ�����
JWPlayer 7����ȷʹ�÷�����Flash plugin failed to load�������
qq_33374003: ��ã�Ϊʲô�Ҳ���&lt;script src=&quot;jwplayer.flash.swf&q...
WebRTCѧϰ֮ʮ����򵥵���Ƶ���죨ʹ��WebRtcVideoEngine2��
�Ӹ��: @dennis_guo:�еģ���������ֻ֧����Ƶ����֧��������Ū���˾��ϴ�
ͨ��live555ʵ��H264 RTSPֱ����Windows�棩
�Ӹ��: @kezanxie3494:�Լ��ĸľͿ����˰���������ͷ�Ļ��;Ͱ���Ƶ�������H264�����ˣ��ȶ���...
��˾���|������ʿ|������|��ϵ��ʽ|��Ȩ����|���ɹ���|���ⱨ��|�������|��̳����
��վ�ͷ���־�ͷ�΢���ͷ�webmaster@csdn.net400-660-0108|����������֪��Ϣ�������޹�˾ ��Ȩ����|����֪֮Ϊ��������޹�˾|������֪���缼�����޹�˾
�� ICP ֤ 09002463 ��|Copyright ? 1999-2017, CSDN.NET, All Rights Reserved GongshangLogo

