

关闭
灿哥哥的博客
专注技术，热爱分享
 目录视图 摘要视图 订阅
 基于RTP协议的H.264视频传输系统：实现
标签： 流媒体rtph.264
2016-11-02 16:59 1246人阅读 评论(2) 收藏 举报
 分类： 流媒体（51）  
版权声明：本文为灿哥哥http://blog.csdn.net/caoshangpa原创文章，转载请标明出处。
实现的原理：基于RTP协议的H.264视频传输系统：原理
相关文章：
【1】RTP协议分析
【2】jrtplib简介
【3】Qt调用jrtplib实现单播、多播和广播
【4】RTP 有效负载(载荷)类型，RTP Payload Type
【5】H.264（H264）视频文件的制作
【6】H.264格式分析
【7】H.264视频压缩标准
关于RTP Payload Format for H.264 Video一定要参考文档rfc6184，因为rfc3984已经被废弃了，rfc6184从下面两个链接都可以打开。
https://tools.ietf.org/html/rfc6184
https://datatracker.ietf.org/doc/rfc6184/?include_text=1
RTP负载为H.264定义了三种不同的基本的负载结构，接收端可能通过RTP负载的首字节来识别它们。这一个字节类似NALU头的格式，它的类型字段则指出了代表的是哪一种结构，这个字节的结构如下：
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
Type定义如下：
0     没有定义
1-23  NAL单元   单个NAL单元包.
24    STAP-A   单一时间的组合包
25    STAP-B   单一时间的组合包
26    MTAP16   多个时间的组合包
27    MTAP24   多个时间的组合包
28    FU-A     分片的单元
29    FU-B     分片的单元
30-31 没有定义
首字节的类型字段和H.264的NALU头中类型字段的区别是，当Type的值为24~31表示这是一个特别格式的NAL单元，而H.264中，只取1~23是有效的值。下面分别说明这三种负载结构。

一.Single NALU Packet（单一NAL单元模式）
即一个RTP负载仅由首字节和一个NALU负载组成，在本文中对于小于1400字节的NALU便采用这种打包方案。这种情况下首字节类型字段和原始的H.264的NALU头类型字段是一样的。也就是说，在这种情况下RTP的负载是一个完整的NALU。


二. Aggregation Packet（组合封包模式）
在一个RTP中封装多个NALU，对于较小的NALU可以采用这种打包方案，从而提高传输效率。即可能是由多个NALU组成一个RTP包。分别有4种组合方式，STAP-A、STAP-B、MTAP16和MTAP24。那么这里的RTP负载首字节类型值分别是24、25、26和27。本文未涉及这种模式。


三.Fragmentation Units（分片封包模式FUs）
一个NALU封装在多个RTP中，每个RTP负载由首字节（这里实际上是FU indicator，但是它和原首字节的结构一样，这里仍然称首字节）、FU header和NALU负载的一部分组成。在本文中，对于大于1400字节的NALU便采用这种方案进行拆包处理。存在两种类型FU-A和FU-B，类型值分别是28和29。
FU-A类型如下图所示：
本文使用的是FU-A类型。

FU-B类型如下图所示：

与FU-A相比，FU-B多了一个DON（decoding order number），DON使用的是网络字节序。FU-B只能用于隔行扫描封包模式，不能用于其他方面。
FU indicator字节结构如下所示：
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
Type=28或29
FU header字节结构如下所示：
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
S（Start）: 1 bit，当设置成1，该位指示分片NAL单元的开始。当随后的FU负载不是分片NAL单元的开始，该位设为0。
E（End）: 1 bit，当设置成1, 该位指示分片NAL单元的结束，此时荷载的最后字节也是分片NAL单元的最后一个字节。当随后的FU荷载不是分片NAL单元的结束,该位设为0。
R（Reserved）: 1 bit，保留位必须设置为0，且接收者必须忽略该位。
Type：与NALU头中的Type值相同

四.使用socket套接字发送RTP打包的H264
主要代码如下所示：
[cpp] view plain copy
int main()  
{  
    OpenBitstreamFile("480320.264");  
    NALU_t *n;  
    char sendbuf[1500];  
  
    unsigned short seq_num =0;  
    int bytes=0;  
    InitWinsock(); //初始化套接字库  
    int    sockfd;  
    struct sockaddr_in addr_in;  
    float framerate=25;  
    unsigned int timestamp_increse=0,ts_current=0;  
    timestamp_increse=(unsigned int)(90000.0 / framerate);  
  
    addr_in.sin_family=AF_INET;  
    addr_in.sin_port=htons(DEST_PORT);  
    addr_in.sin_addr.s_addr=inet_addr(DEST_IP);  
    sockfd=socket(AF_INET,SOCK_DGRAM,0);  
    connect(sockfd, (const sockaddr *)&addr_in, sizeof(sockaddr_in)) ;//申请UDP套接字  
    n = AllocNALU(8000000);//为结构体nalu_t及其成员buf分配空间。返回值为指向nalu_t存储空间的指针  
  
    while(!feof(bits))  
    {  
        GetAnnexbNALU(n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001  
        dump(n);//输出NALU长度和TYPE  
  
        memset(sendbuf,0,1500);//清空sendbuf；此时会将上次的时间戳清空，因此需要ts_current来保存上次的时间戳值  
  
        //rtp固定包头，为12字节,该句将sendbuf[0]的地址赋给rtp_hdr，以后对rtp_hdr的写入操作将直接写入sendbuf。  
        rtp_hdr =(RTP_FIXED_HEADER*)&sendbuf[0];  
  
        //设置RTP HEADER  
        rtp_hdr->version = 2;   //版本号，此版本固定为2  
        rtp_hdr->marker  = 0;   //标志位，由具体协议规定其值。  
        rtp_hdr->payload = H264;//负载类型号，  
        rtp_hdr->ssrc    = htonl(10);//随机指定为10，并且在本RTP会话中全局唯一  
  
        //当一个NALU小于1400字节的时候，采用一个单RTP包发送  
        if(n->len<=MAX_RTP_PKT_LENGTH)  
        {  
            //设置rtp M 位；  
            rtp_hdr->marker=1;  
            rtp_hdr->seq_no = htons(seq_num ++); //序列号，每发送一个RTP包增1  
            ts_current=ts_current+timestamp_increse;  
            rtp_hdr->timestamp=htonl(ts_current);  
            //设置NALU HEADER,并将这个HEADER填入sendbuf[12]  
            nalu_hdr =(NALU_HEADER*)&sendbuf[12]; //将sendbuf[12]的地址赋给nalu_hdr，之后对nalu_hdr的写入就将写入sendbuf中；  
            nalu_hdr->F=n->forbidden_bit;  
            nalu_hdr->NRI=n->nal_reference_idc>>5;//有效数据在n->nal_reference_idc的第6，7位，需要右移5位才能将其值赋给nalu_hdr->NRI。  
            nalu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[13],n->buf+1,n->len-1);//去掉nalu头的nalu剩余内容写入sendbuf[13]开始的字符串。  
            bytes=n->len + 12 ;  //获得sendbuf的长度,为nalu的长度（包含NALU头但除去起始前缀）加上rtp_header的固定长度12字节  
            if(n->nal_unit_type==1 || n->nal_unit_type==5)  
            {  
  
                send(sockfd,sendbuf,bytes,0);//发送RTP包  
            }  
            else  
            {  
                send(sockfd,sendbuf,bytes,0);//发送RTP包  
                  //如果是6,7类型的包，不应该延时；之前有停顿，原因这在这  
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
  
            //发送第一个的FU，S=1，E=0，R=0  
            rtp_hdr->seq_no = htons(seq_num ++); //序列号，每发送一个RTP包增1  
            //设置rtp M 位；  
            rtp_hdr->marker=0;  
            //设置FU INDICATOR,并将这个HEADER填入sendbuf[12]  
            fu_ind =(FU_INDICATOR*)&sendbuf[12]; //将sendbuf[12]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;//使用FU-A  
  
            //设置FU HEADER,并将这个HEADER填入sendbuf[13]  
            fu_hdr =(FU_HEADER*)&sendbuf[13];  
            fu_hdr->S=1;  
            fu_hdr->E=0;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[14],n->buf+1,MAX_RTP_PKT_LENGTH);//去掉NALU头  
            bytes=MAX_RTP_PKT_LENGTH+14;//获得sendbuf的长度,为nalu的长度（除去起始前缀和NALU头）加上rtp_header，fu_ind，fu_hdr的固定长度14字节  
            send( sockfd, sendbuf, bytes, 0 );//发送RTP包  
  
            //发送中间的FU，S=0，E=0，R=0  
            for(packetIndex=2;packetIndex<packetNum;packetIndex++)  
            {  
                rtp_hdr->seq_no = htons(seq_num ++); //序列号，每发送一个RTP包增1  
  
                //设置rtp M 位；  
                rtp_hdr->marker=0;  
                //设置FU INDICATOR,并将这个HEADER填入sendbuf[12]  
                fu_ind =(FU_INDICATOR*)&sendbuf[12]; //将sendbuf[12]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
                fu_ind->F=n->forbidden_bit;  
                fu_ind->NRI=n->nal_reference_idc>>5;  
                fu_ind->TYPE=28;  
  
                //设置FU HEADER,并将这个HEADER填入sendbuf[13]  
                fu_hdr =(FU_HEADER*)&sendbuf[13];  
                fu_hdr->S=0;  
                fu_hdr->E=0;  
                fu_hdr->R=0;  
                fu_hdr->TYPE=n->nal_unit_type;  
  
                memcpy(&sendbuf[14],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);//去掉起始前缀的nalu剩余内容写入sendbuf[14]开始的字符串。  
                bytes=MAX_RTP_PKT_LENGTH+14;//获得sendbuf的长度,为nalu的长度（除去原NALU头）加上rtp_header，fu_ind，fu_hdr的固定长度14字节  
                send( sockfd, sendbuf, bytes, 0 );//发送rtp包  
            }  
  
            //发送最后一个的FU，S=0，E=1，R=0  
            rtp_hdr->seq_no = htons(seq_num ++);  
            //设置rtp M 位；当前传输的是最后一个分片时该位置1  
            rtp_hdr->marker=1;  
            //设置FU INDICATOR,并将这个HEADER填入sendbuf[12]  
            fu_ind =(FU_INDICATOR*)&sendbuf[12]; //将sendbuf[12]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;  
  
            //设置FU HEADER,并将这个HEADER填入sendbuf[13]  
            fu_hdr =(FU_HEADER*)&sendbuf[13];  
            fu_hdr->S=0;  
            fu_hdr->E=1;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[14],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,lastPackSize-1);//将nalu最后剩余的-1(去掉了一个字节的NALU头)字节内容写入sendbuf[14]开始的字符串。  
            bytes=lastPackSize-1+14;//获得sendbuf的长度,为剩余nalu的长度减1加上rtp_header，FU_INDICATOR,FU_HEADER三个包头共14字节  
            send( sockfd, sendbuf, bytes, 0 );//发送rtp包  
        }  
  
       Sleep(40);  
    }  
  
    FreeNALU(n);  
#ifdef WIN32  
    WSACleanup();  
#endif // WIN32  
    return 0;  
}  

五.使用jrtplib发送RTP打包的H264
使用jrtplib的好处是，不需要自己再添加RTP头了，而且jrtplib内部实现了RTCP协议。jrtplib的简介可以参考【2】，使用方法可以参考【3】。
主要代码如下所示。
[cpp] view plain copy
int main()  
{  
    //初始化jrtplib  
    uint8_t destIP[]={127,0,0,1};  
    initialRTP(destIP);  
    //打开264文件  
    OpenBitstreamFile("480320.264");  
    NALU_t *n;  
    char sendbuf[1500];  
    int bytes=0;  
  
    n = AllocNALU(8000000);//为结构体nalu_t及其成员buf分配空间。返回值为指向nalu_t存储空间的指针  
  
    while(!feof(bits))  
    {  
        GetAnnexbNALU(n);//每执行一次，文件的指针指向本次找到的NALU的末尾，下一个位置即为下个NALU的起始码0x000001  
        dump(n);//输出NALU长度和TYPE  
  
        memset(sendbuf,0,1500);//清空sendbuf  
  
        //当一个NALU小于1400字节的时候，采用一个单RTP包发送  
        if(n->len<=MAX_RTP_PKT_LENGTH)  
        {  
  
            //设置NALU HEADER,并将这个HEADER填入sendbuf[0]  
            nalu_hdr =(NALU_HEADER*)&sendbuf[0]; //将sendbuf[0]的地址赋给nalu_hdr，之后对nalu_hdr的写入就将写入sendbuf中；  
            nalu_hdr->F=n->forbidden_bit;  
            nalu_hdr->NRI=n->nal_reference_idc>>5;//有效数据在n->nal_reference_idc的第6，7位，需要右移5位才能将其值赋给nalu_hdr->NRI。  
            nalu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[1],n->buf+1,n->len-1);//去掉nalu头的nalu剩余内容写入sendbuf[1]开始的字符串。  
            bytes=n->len;  
            if(n->nal_unit_type==1 || n->nal_unit_type==5)  
            {  
                //第三参数Mark即RTP头中的M位  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,true,3600);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
            }  
            else  
            {  
                //注意此处与使用socekt发送的不同，此时时间戳不增加  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,true,0);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
                  //如果是6,7类型的包，不应该延时；之前有停顿，原因这在这  
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
  
  
            //发送第一个的FU，S=1，E=0，R=0  
            fu_ind =(FU_INDICATOR*)&sendbuf[0]; //将sendbuf[0]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;//使用FU-A  
  
            //设置FU HEADER,并将这个HEADER填入sendbuf[1]  
            fu_hdr =(FU_HEADER*)&sendbuf[1];  
            fu_hdr->S=1;  
            fu_hdr->E=0;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[2],n->buf+1,MAX_RTP_PKT_LENGTH);//去掉NALU头  
            bytes=MAX_RTP_PKT_LENGTH+2;//获得sendbuf的长度,为nalu的长度（除去起始前缀和NALU头）加fu_ind，fu_hdr的固定长度2字节  
            int status = m_session.SendPacket((void *)sendbuf,bytes,96,false,0);  
            if (status < 0)  
            {  
                std::cout<<RTPGetErrorString(status)<<std::endl;  
            }  
  
            //发送中间的FU，S=0，E=0，R=0  
            for(packetIndex=2;packetIndex<packetNum;packetIndex++)  
            {  
                //设置FU INDICATOR,并将这个HEADER填入sendbuf[0]  
                fu_ind =(FU_INDICATOR*)&sendbuf[0]; //将sendbuf[0]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
                fu_ind->F=n->forbidden_bit;  
                fu_ind->NRI=n->nal_reference_idc>>5;  
                fu_ind->TYPE=28;  
  
                //设置FU HEADER,并将这个HEADER填入sendbuf[1]  
                fu_hdr =(FU_HEADER*)&sendbuf[1];  
                fu_hdr->S=0;  
                fu_hdr->E=0;  
                fu_hdr->R=0;  
                fu_hdr->TYPE=n->nal_unit_type;  
  
                memcpy(&sendbuf[2],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,MAX_RTP_PKT_LENGTH);//去掉起始前缀的nalu剩余内容写入sendbuf[24]开始的字符串。  
                bytes=MAX_RTP_PKT_LENGTH+2;//获得sendbuf的长度,为nalu的长度（除去原NALU头）加上fu_ind，fu_hdr的固定长度2字节  
                int status = m_session.SendPacket((void *)sendbuf,bytes,96,false,0);  
                if (status < 0)  
                {  
                    std::cout<<RTPGetErrorString(status)<<std::endl;  
                }  
            }  
  
            //设置FU INDICATOR,并将这个HEADER填入sendbuf[0]  
            fu_ind =(FU_INDICATOR*)&sendbuf[0]; //将sendbuf[0]的地址赋给fu_ind，之后对fu_ind的写入就将写入sendbuf中；  
            fu_ind->F=n->forbidden_bit;  
            fu_ind->NRI=n->nal_reference_idc>>5;  
            fu_ind->TYPE=28;  
  
            //设置FU HEADER,并将这个HEADER填入sendbuf[1]  
            fu_hdr =(FU_HEADER*)&sendbuf[1];  
            fu_hdr->S=0;  
            fu_hdr->E=1;  
            fu_hdr->R=0;  
            fu_hdr->TYPE=n->nal_unit_type;  
  
            memcpy(&sendbuf[2],n->buf+(packetIndex-1)*MAX_RTP_PKT_LENGTH+1,lastPackSize-1);//将nalu最后剩余的-1(去掉了一个字节的NALU头)字节内容写入sendbuf[2]开始的字符串。  
            bytes=lastPackSize-1+2;//获得sendbuf的长度,为剩余nalu的长度减1加上FU_INDICATOR,FU_HEADER两个包头共2字节  
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

六.测试
上述两种方法的测试效果相同。打开VLC或者MPlayer播放器（强烈推荐MPlayer，效果比VLC好），将w.sdp拖入到播放器中，播放器进入等待状态，然后运行程序发送RTP流，播放器开始播放。
测试使用了176144.264和480320.264两个H.264文件，第一个分辨率为176*144，NALU比较小，打包时不需要分段；第二个分辨率480*320，NALU大于1400，固存在分段打包的情况。
w.sdp
[cpp] view plain copy
m=video 25000 RTP/AVP 96     
a=rtpmap:96 H264             
a=framerate:25               
c=IN IP4 127.0.0.1           
注意端口号25000要和程序中设置的相同。
测试效果如下所示：



七.小结
关于时间戳，需要注意的是h264的采样率为90000HZ，因此时间戳的单位为1(秒)/90000，因此如果当前视频帧率为25fps，那时间戳间隔或者说增量应该为3600，如果帧率为30fps，则增量为3000，以此类推。
关于h264拆包，按照FU-A方式说明：
1.第一个FU-A包的FU indicator：F应该为当前NALU头的F，而NRI应该为当前NALU头的NRI，Type则等于28，表明它是FU-A包。FU header生成方法：S = 1，E = 0，R = 0，Type则等于NALU头中的Type。
2.后续的N个FU-A包的FU indicator和第一个是完全一样的，如果不是最后一个包，则FU header应该为：S = 0，E = 0，R = 0，Type等于NALU头中的Type。
3.最后一个FU-A包FU header应该为：S = 0，E = 1，R = 0，Type等于NALU头中的Type。
因此总结就是：同一个NALU分包厚的FU indicator头是完全一致的，FU header只有S以及E位有区别，分别标记开始和结束，它们的RTP分包的序列号应该是依次递增的，并且它们的时间戳必须一致，而负载数据为NALU包去掉1个字节的NALU头后对剩余数据的拆分，这点很关键，你可以认为NALU头被拆分成了FU indicator和FU header，所以不再需要1字节的NALU头了。
关于SPS以及PPS，配置帧的传输我采用了先发SPS，再发送PPS，并使用同样的时间戳，或者按照正常时间戳增量再或者组包发送的形式处理貌似都可以，看播放器怎么解码了，另外提一下，如果我们使用vlc进行播放的话，可以在sdp文件中设置SPS以及PPS，这样就可以不用发送它们了。
使用VLC播放时，sdp文件中的分包模式选项：packetization-mode=1，即在文件末尾加入=fmtp:packetization-mode=1，否则有问题。
关于packetization-mode的详细说明如下：
当packetization-mode的值为0时或不存在时, 必须使用单一NALU单元模式。
当packetization-mode的值为1时必须使用非交错(non-interleaved)封包模式。
当packetization-mode的值为2时必须使用交错(interleaved)封包模式。
每个打包方式允许的NAL单元类型总结(yes = 允许, no = 不允许, ig = 忽略)
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

源码链接：见http://blog.csdn.net/caoshangpa/article/details/53009604的评论

顶
2
 
踩
0
 
 
上一篇RTP 有效负载(载荷)类型，RTP Payload Type
下一篇H.264视频压缩标准
  相关文章推荐
? 实现RTP协议的H.264视频传输系统
? rtp c++ 网络视频传输
? RTP网络视频传输
? RTP协议: Internet音视频传输
? RTP协议的H.264视频传输
? 基于JMF RTP的音视频传输
? H.264视频的RTP荷载格式
? H264关于RTP协议的实现
? 基于DirectShow的MPEG-4视频传输系统的研究与实现
? [转帖]H.264 RTP payload 格式(有效载荷)
猜你在找
机器学习之概率与统计推断 机器学习之数学基础 机器学习之凸优化 机器学习之矩阵 响应式布局全新探索 探究Linux的总线、设备、驱动模型 深度学习基础与TensorFlow实践 深度学习之神经网络原理与实战技巧 前端开发在线峰会 TensorFlow实战进阶：手把手教你做图像识别应用
查看评论
2楼 arbotor 2016-12-06 01:42发表 [回复] 感谢楼主的无私分享，有个问题想请教一下，我是直接下楼主的工程直接运行的，可是编译器总是报错：F:\Qt RTP H264\testJrtplib\main.cpp:277: error: undefined reference to `_imp___ZN7jrtplib17RTPGetErrorStringB5cxx11Ei'，这是什么原因呢？我找了好久也没找到原因
1楼 灿哥哥 2016-11-11 10:55发表 [回复] [引用] [举报]源码链接：http://download.csdn.net/detail/caoshangpa/9679976
发表评论
用 户 名：
yjduoduo
评论内容：
插入代码

提交  
* 以上用户言论只代表其个人观点，不代表CSDN网站的观点或立场
个人资料
访问我的空间  
灿哥哥
 
 1  3
访问：284205次
积分：6480
等级： 
排名：第3513名
原创：143篇转载：123篇译文：0篇评论：252条
文章搜索

 搜索
博客专栏
	WebRTC学习
文章：11篇
阅读：13050
文章分类
Qt(86)
Python(1)
3D(4)
other(52)
设计模式(24)
JavaScript(1)
C/C++(14)
libevent(3)
学习现代3D图形编程(4)
C++11并发学习(6)
PhotoShop(2)
流媒体(52)
优秀开源项目(3)
WebRTC(11)
文章存档
2017年06月(8)
2017年05月(6)
2017年04月(3)
2017年03月(8)
2017年01月(11)
展开
阅读排行
JWPlayer 7的正确使用方法及Flash plugin failed to load解决方法(8850)
Qt生成随机数的方法(7569)
Qt C++定义全局变量的两种方式(6842)
TCP、UDP数据包大小的限制(6290)
Qt串口通信接收数据不完整的解决方法(6038)
Qt加载百度离线地图(5086)
使用Qt5.7.0 VS2015版本生成兼容XP的可执行程序(4541)
使用VLC浏览器插件播放RTSP流(4379)
Adobe Media Server 5（AMS）的安装及使用(4145)
FFmpeg支持的音频和视频编解码格式(3855)
评论排行
Qt加载百度离线地图(33)
Qt基于FFmpeg播放本地 H.264（H264）文件(16)
通过live555实现H264 RTSP直播（Windows版）(13)
Qt播放PCM音频（裸流）文件的两种方法(11)
Qt调用jrtplib实现单播、多播和广播(9)
Windows 7（Win7）下MinGW+msys编译ffmpeg，并加入H264编码支持(6)
Qt实现基于G.729A（G729A）的语音聊天(6)
使用Qt5.7.0 VS2015版本生成兼容XP的可执行程序(5)
VS2015编译Qt5.7.0生成支持XP的静态库(5)
Qt带进度条的启动界面(5)
推荐文章
* CSDN日报20170706——《屌丝程序员的逆袭之旅》
* 探讨后端选型中不同语言及对应的Web框架
* 细说反射，Java 和 Android 开发者必须跨越的坎
* 深度学习 | 反向传播与它的直观理解
* ArcGIS 水文分析实战教程——雨量计算与流量统计
* 每周荐书：Android、Keras、ES6（评论送书）
最新评论
C++笔试题 String类的实现
qq_35434315: 可以
Qt基于FFmpeg播放本地 H.264（H264）文件
小葱_哥: @qq_36685978:你好，我也是出来了UI界面，没有图像，请问你是怎么解决的？
Qt基于FFmpeg播放本地 H.264（H264）文件
小葱_哥: @qq_36685978:你好，我也是编译好后没有图像，请问你是怎么解决的？
WebRTC学习之八：函数的异步执行
仁生不言败: 同问，为何要实现消息循环。而且这样实现，关闭窗口了，程序其实还没结束
JWPlayer 7的正确使用方法及Flash plugin failed to load解决方法
陈小妹: 我用的是xampp作为服务器 但还是播放不了flv的视频 怎么办
Qt基于FFmpeg播放本地 H.264（H264）文件
减肥目测你: 这个是一帧一帧解析还是说一个gop一个gop解析啊，我P帧的话是不是应该和I帧一起解析啊？
Qt基于FFmpeg播放本地 H.264（H264）文件
减肥目测你: 实时传输264码流，会出现延迟加卡顿的现象
JWPlayer 7的正确使用方法及Flash plugin failed to load解决方法
qq_33374003: 你好，为什么我不加&lt;script src=&quot;jwplayer.flash.swf&q...
WebRTC学习之十：最简单的视频聊天（使用WebRtcVideoEngine2）
灿哥哥: @dennis_guo:有的，但是现在只支持视频，不支持语音，弄完了就上传
通过live555实现H264 RTSP直播（Windows版）
灿哥哥: @kezanxie3494:自己改改就可以了啊，传摄像头的话就就把视频流编码成H264就行了，比读本...
公司简介|招贤纳士|广告服务|联系方式|版权声明|法律顾问|问题报告|合作伙伴|论坛反馈
网站客服杂志客服微博客服webmaster@csdn.net400-660-0108|北京创新乐知信息技术有限公司 版权所有|江苏知之为计算机有限公司|江苏乐知网络技术有限公司
京 ICP 证 09002463 号|Copyright ? 1999-2017, CSDN.NET, All Rights Reserved GongshangLogo

