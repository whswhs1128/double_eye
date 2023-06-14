#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/videodev2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "rtsp_demo.h"
#include "comm.h"
#include "sample_comm.h"

#define VIDEO_NAME				"/dev/video0"
#define TEST_STREAM_SAVE_PATH	"."
#define BUFFER_NUM				(4)
#define V4L2_BUF_TYPE  			(V4L2_BUF_TYPE_VIDEO_CAPTURE)
static int g_video_fd;

typedef struct{
    void *start;
	int length;
}BUFTYPE;

 
BUFTYPE *usr_buf;
static unsigned int n_buffer = 0;
 

static int HI_PDT_Camera_Open(void)
{
	struct v4l2_input inp;
 	int i = 0;
	int ret = -1;
	g_video_fd = open(VIDEO_NAME, O_RDWR | O_NONBLOCK,0);
	if(g_video_fd < 0)
	{	
		printf("%s open failed ! \n", VIDEO_NAME);
		return ret;
	};

	for(i=0;i<16;i++)
	{
		inp.index = i;
		if (-1 == ioctl (g_video_fd, VIDIOC_S_INPUT, &inp))
		{
			printf("VIDIOC_S_INPUT  failed %d !\n",i);
		}
		else
		{
			printf("VIDIOC_S_INPUT  success %d !\n",i);
			ret = 0;
			break;
		}
	}
 
	return ret;
}

// close 
void HI_PDT_Camera_Close(int video_fd)
{
	unsigned int i;

	for(i = 0;i < n_buffer; i++)
	{
		if(-1 == munmap(usr_buf[i].start,usr_buf[i].length))
		{
			exit(-1);
		}
	}
	
	if(NULL != usr_buf)
	free(usr_buf);
	
 	if(video_fd >0)
		close(video_fd);
		
	return;
}
 
 
/*set video capture ways(mmap)*/
int HI_PDT_Init_mmap(int fd)
{
	/*to request frame cache, contain requested counts*/
	struct v4l2_requestbuffers reqbufs;
 
	memset(&reqbufs, 0, sizeof(reqbufs));
	reqbufs.count = BUFFER_NUM; 	 							/*the number of buffer*/
	reqbufs.type = V4L2_BUF_TYPE;    
	reqbufs.memory = V4L2_MEMORY_MMAP;				
 
	if(-1 == ioctl(fd,VIDIOC_REQBUFS,&reqbufs))
	{
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		return -1;
	}
	
	n_buffer = reqbufs.count;
	printf("n_buffer = %d\n", n_buffer);
	usr_buf = calloc(reqbufs.count, sizeof(BUFTYPE));
	if(usr_buf == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
 
	/*map kernel cache to user process*/
	for(n_buffer = 0; n_buffer < reqbufs.count; n_buffer++)
	{
		//stand for a frame
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffer;
		
		/*check the information of the kernel cache requested*/
		if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf))
		{
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			return -1;
		}
 
		usr_buf[n_buffer].length = buf.length;
		usr_buf[n_buffer].start = (char *)mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED, fd,buf.m.offset);
 
		if(MAP_FAILED == usr_buf[n_buffer].start)
		{
			perror("Fail to mmap");
			return -1;
		}
 
	}
	
	return 0;
 
}

static int HI_PDT_Set_Format(int video_fd)
{
	struct v4l2_format 		tv_fmt; /* frame format */  

 	/*set the form of camera capture data*/
	tv_fmt.type = V4L2_BUF_TYPE;      /*v4l2_buf_typea,camera must use V4L2_BUF_TYPE_VIDEO_CAPTURE*/
	tv_fmt.fmt.pix.width = 640;
	tv_fmt.fmt.pix.height = 480;
	tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	tv_fmt.fmt.pix.field = V4L2_FIELD_NONE;   		/*V4L2_FIELD_NONE*/
	if (ioctl(video_fd, VIDIOC_S_FMT, &tv_fmt)< 0) 
	{
		fprintf(stderr,"VIDIOC_S_FMT set err\n");
		return -1;
	}
	return 0;

}
static int HI_PDT_Init_Camera(int video_fd)
{
	struct v4l2_capability 	cap;	/* decive fuction, such as video input */
	struct v4l2_fmtdesc 	fmtdesc;  	/* detail control value */

	int ret = 0;
	if(video_fd <=0)
		return -1;
	
			/*show all the support format*/
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0 ;                 /* the number to check */
	fmtdesc.type=V4L2_BUF_TYPE;
 
	/* check video decive driver capability */
	if(ret=ioctl(video_fd, VIDIOC_QUERYCAP, &cap)<0)
	{
		fprintf(stderr, "fail to ioctl VIDEO_QUERYCAP \n");
		return -1;
	}
	
	/*judge wherher or not to be a video-get device*/
	if(!(cap.capabilities & V4L2_BUF_TYPE))
	{
		fprintf(stderr, "The Current device is not a video capture device \n");
		return -1;
	}
 
	/*judge whether or not to supply the form of video stream*/
	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("The Current device does not support streaming i/o\n");
		return -1;
	}
	
	printf("\ncamera driver name is : %s\n",cap.driver);
	printf("camera device name is : %s\n",cap.card);
	printf("camera bus information: %s\n",cap.bus_info);
 
	/*display the format device support*/
	printf("\n");
	while(ioctl(video_fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
	{	
		printf("support device %d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}
	printf("\n");
 

 	return 0;
}
 
int HI_PDT_start_capture(int fd)
{
	unsigned int i;
	enum v4l2_buf_type type;
	
	/*place the kernel cache to a queue*/
	for(i = 0; i < n_buffer; i++)
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
 
		if(-1 == ioctl(fd, VIDIOC_QBUF, &buf))
		{
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}
 
	type = V4L2_BUF_TYPE;
	if(-1 == ioctl(fd, VIDIOC_STREAMON, &type))
	{
		printf("i=%d.\n", i);
		perror("VIDIOC_STREAMON");
		close(fd);
		exit(EXIT_FAILURE);
	}
 
	return 0;
}
 
 
 int frame = 0;
VB_BLK handleY = VB_INVALID_HANDLE;
VB_POOL poolID;
HI_U64 phyYaddr;
// HI_U64 *pVirYaddr;

#if 1
int YUV422To420(unsigned char yuv422[], unsigned char yuv420[], int width, int height)  
{          
  
       int ynum=width*height;  
       int i,j,k=0;  
    //??Y??  
       for(i=0;i<ynum;i++){  
           yuv420[i]=yuv422[i*2];  
       }  
    //??U??  
    //    for(i=0;i<height;i++){  
    //        if((i%2)!=0)continue;  
    //        for(j=0;j<(width/2);j++){  
    //            if((4*j+1)>(2*width))break;  
    //            yuv420[ynum+k*2*width/4+j]=yuv422[i*2*width+4*j+1];  
    //                    }  
    //         k++;  
    //    }  
    //    k=0;  
    // //??V??  
    //    for(i=0;i<height;i++){  
    //        if((i%2)==0)continue;  
    //        for(j=0;j<(width/2);j++){  
    //            if((4*j+3)>(2*width))break;  
    //            yuv420[ynum+ynum/4+k*2*width/4+j]=yuv422[i*2*width+4*j+3];  
                
    //        }  
    //         k++;  
    //    }  
         
         
       return 1;  
}  
#endif

#if 0
int YUV422TO420SP(char *yuv422, char *yuv420,int i32Height ,int i32Width )
{
	
	//  if( yuv422 == nullptr || yuv420 == nullptr )
    // {
    //     return;
    // }
    
    char *y = yuv420;
    char *uv = yuv420+ i32Width * i32Height ;
    char *start = yuv422;

    int i = 0, j = 0, GetUvFlag = 0;

    for (int row = 0; row < i32Width * i32Height * 2; )
    {
        y[i++]      = start[row];        
        y[i++]      = start[row+2];

        //?????
        if(GetUvFlag%2 == 0)
		{
			uv[j]     = start[row+1];//u
			uv[j+1]     	= start[row+3];//v
			j+=2;
		}

		row+=4;

        if(row%(i32Width*2) == 0)
		{
			GetUvFlag++;
		}
    }
	return 1;
}
#endif

unsigned char buf_yyy[640 * 480];
int HI_PDT_process_image(void *addr, int length)
{
	static int num = 0;
	HI_S32 s32Ret = 0;
	HI_U64 *pVirYaddr;
	HI_U64 pu64CurPTS;

				// FILE *fp_tmp;
				// char image_name[64] = {0};
				// sprintf(image_name, "saved.yuv");
				// 	if((fp_tmp = fopen(image_name, "w")) == NULL)
				// 	{
				// 		perror("Fail to fopen \n");
				// 		exit(EXIT_FAILURE);
				// 	}
				// fread(addr, 1, length, fp_tmp);

	if(1) {
		memset(buf_yyy, 0, 640*480);
		YUV422To420(addr,buf_yyy,640,480);	
		// sleep(2);
	}


	VIDEO_FRAME_INFO_S *pstFrame = malloc(sizeof(VIDEO_FRAME_INFO_S));
    

        //         if( handleY == VB_INVALID_HANDLE)
        //         {
        //                 printf("getblock for y failed\n");
        //                 return -1;
        //         }
        //         else {
        //     // printf("handleY is %d\n", handleY);
        // 		}
        //         VB_POOL poolID =  HI_MPI_VB_Handle2PoolId (handleY);//µÃµ½poolID
        // // printf("pool ID = %d\n", poolID);

        //         phyYaddr = HI_MPI_VB_Handle2PhysAddr(handleY);
        //         if( phyYaddr == 0)
        //         {
        //                 printf("HI_MPI_VB_Handle2PhysAddr for handleY failed\n");
        //                 return -1;
        //         }
               
                pVirYaddr = (HI_U64 *) HI_MPI_SYS_Mmap(phyYaddr, 640 * 480 );
				// printf("pVirYaddr %p  %x \n",pVirYaddr,pVirYaddr);

	        memset(&(pstFrame->stVFrame),0x00,sizeof(VIDEO_FRAME_S));
                pstFrame->stVFrame.u32Width = 640;
                pstFrame->stVFrame.u32Height = 480;
                pstFrame->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_400;//;
                pstFrame->u32PoolId = poolID;
        		pstFrame->enModId = HI_ID_VENC;
                pstFrame->stVFrame.u64PhyAddr[0] = phyYaddr;
                // pstFrame->stVFrame.u64PhyAddr[1] = phyYaddr + 640 * 480;
                pstFrame->stVFrame.u64VirAddr[0] = (HI_U64)(HI_UL)pVirYaddr;
                // pstFrame->stVFrame.u64VirAddr[1] = (HI_U64)(HI_UL)pVirYaddr + 640 * 480;
                pstFrame->stVFrame.u32Stride[0] = 640 ;
                // pstFrame->stVFrame.u32Stride[1] = 640 ;
                pstFrame->stVFrame.enField     = VIDEO_FIELD_FRAME;
                pstFrame->stVFrame.enCompressMode = COMPRESS_MODE_NONE;
                pstFrame->stVFrame.enVideoFormat  = VIDEO_FORMAT_LINEAR;
        		pstFrame->stVFrame.enDynamicRange = DYNAMIC_RANGE_SDR8;
        		pstFrame->stVFrame.enColorGamut = COLOR_GAMUT_BT709;
				HI_MPI_SYS_GetCurPTS(&pu64CurPTS);
                pstFrame->stVFrame.u64PTS     = pu64CurPTS;
        		pstFrame->stVFrame.u32TimeRef = frame * 2;
				frame ++;
				memcpy(pVirYaddr, buf_yyy, 640*480);
				// memcpy(pVirYaddr, addr, 640*480);

			// if(frame == 50) {
			// 	FILE *fp;
			// 	char image_name[64] = {0};
			// 	sprintf(image_name, "saved.yuv");
			// 		if((fp = fopen(image_name, "w")) == NULL)
			// 		{
			// 			perror("Fail to fopen \n");
			// 			exit(EXIT_FAILURE);
			// 		}
			// 		fwrite(&buf_yyy, length, 1, fp);
			// 		printf("saved ok...\n");
			// 		usleep(500);
			// 		fclose(fp);

			// }
		// printf("frame is %d\n", frame);
	s32Ret = HI_MPI_VENC_SendFrame(4, pstFrame, -1);
	// s32Ret = HI_MPI_VPSS_SendFrame(0, 0, pstFrame, -1);
	if(s32Ret < 0)
        {
            printf("HI_MPI_VENC_SendFrame failed, errorcode is %#x!\n", s32Ret);
            return -1;
        } else {
			//  printf("HI_MPI_VENC_SendFrame success\n");
		}

		HI_MPI_SYS_Munmap(pVirYaddr, 640 * 480);
		free(pstFrame);
	


	return 0;
}
 
int HI_PDT_read_frame(int fd)
{
	struct v4l2_buffer buf;

	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE;
	buf.memory = V4L2_MEMORY_MMAP;

	//put cache from queue
	if(-1 == ioctl(fd, VIDIOC_DQBUF,&buf))
	{
		perror("Fail to ioctl 'VIDIOC_DQBUF'");
		return -1;
	}
	if(buf.index >= n_buffer)
		return -1;
 
	//read process space's data to a file
	HI_PDT_process_image(usr_buf[buf.index].start, usr_buf[buf.index].length);
	if(-1 == ioctl(fd, VIDIOC_QBUF,&buf))
	{
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		return -1;
	}

	
	
	
	return 0;
}
 
 

 
static int HI_PDT_Stop_Capture(int fd)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE;
	if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type))
	{
		perror("Fail to ioctl 'VIDIOC_STREAMOFF' \n");
		return -1;
	}
	return 0;
}
 

int init_vb_handle()
{
	handleY = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, 640 * 480 , NULL);
	if (VB_INVALID_HANDLE == handleY) {
                SAMPLE_PRT("handleY is VB_INVALID_HANDLE\n");
            }

	VB_POOL poolID =  HI_MPI_VB_Handle2PoolId (handleY);//??poolID
	// phyYaddr = HI_MPI_VB_Handle2PhysAddr(handleY);
	        phyYaddr = HI_MPI_VB_Handle2PhysAddr(handleY);
                if( phyYaddr == 0)
                {
                        printf("HI_MPI_VB_Handle2PhysAddr for handleY failed\n");
                        // return -1;
                }
	// pVirYaddr = (HI_U64 *) HI_MPI_SYS_Mmap(phyYaddr, 640 * 480 * 3 / 2);
	*buf_yyy = malloc(640 * 480);
	
}

int HI_PDT_mainloop(int fd)
{
	// int count = 10;
	// while(count-- > 0)
	// {

		init_vb_handle();
		while(1)
		{
			fd_set fds;
			struct timeval tv;
			int r;
 
			FD_ZERO(&fds);
			FD_SET(fd,&fds);
 
			/*Timeout*/
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			r = select(fd + 1,&fds,NULL,NULL,&tv);
			
			if(-1 == r)
			{
				 if(EINTR == errno)
					continue;
				perror("Fail to select \n");
				return -1;
			}
			if(0 == r)
			{
				fprintf(stderr,"select Timeout \n");
				return -1;
			}
 
			if(HI_PDT_read_frame(fd) == 0) {
				continue;;
			}
				
			usleep(40 * 1000);
		}
	// }
	return 0;
}


int sample_uvc_start(HI_VOID)
{	
	int s32Ret =0;
	
	// 1 open device
	s32Ret = HI_PDT_Camera_Open();
	if(s32Ret <0)
	{
		printf("HI_PDT_Camera_Open failed ! \n");
		return -1;
	}

	// Check and set device properties  set frame 
	s32Ret = HI_PDT_Init_Camera(g_video_fd);
	if(s32Ret <0)
	{
		printf("HI_PDT_Camera_Open failed ! \n");
		HI_PDT_Camera_Close(g_video_fd);
		return -1;
	}
	
	HI_PDT_Set_Format(g_video_fd);

	// Apply for a video buffer 
	HI_PDT_Init_mmap(g_video_fd);

	// 
	HI_PDT_start_capture(g_video_fd);

	
	HI_PDT_mainloop(g_video_fd);

	
	HI_PDT_Stop_Capture(g_video_fd);
	
	return 0;
}


int HI_PDT_UVC_DeInit(HI_VOID)
{	
	
	HI_PDT_Camera_Close(g_video_fd);
		
	HI_PDT_Stop_Capture(g_video_fd);

	return 0;
}


