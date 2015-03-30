#include "protocal.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include "config.h"
#include <stdio.h>
#include "util.h"
#include "pro_timer.h"
#include "serial.h"

typedef enum protocal_state_ {
    E_PSTATE_NONE,       
    E_PSTATE_INIT,        
    E_PSTATE_HANDSHAKE,
    E_PSTATE_LINKERROR,
    E_PSTATE_COMMUNICATE, 
}protocal_state;

/* frame receiver, for read searial port */
typedef struct frame_receiver_{
    char  buf1[FRAME_RECEIVER_BUF_LEN + 2];         /* frame buffer 1 */
    int   buf1_datalen;                             /* buffer1 data length */
    bool  bbuf1;                                    /* buffer1 contains a valid frame or not */
    char  buf2[FRAME_RECEIVER_BUF_LEN + 2];         
    int   buf2_datalen;                             
    bool  bbuf2;                   
            
    char  accept_buf[FRAME_RECEIVER_BUF_LEN + 2];   /* derect receiver buffer */
    bool  bincomplete_frame;
    int   pos;
    int   frame_flag_count;

    timer_t timer_frame_syn;
    bool  frame_time_out;
} frame_receiver;

/* frame sender */
typedef struct frame_sender_{
    /* frame cache, store the frame after writed to serial */
  

    char* request_cache;
    int request_cache_len; 
    char* responce_cache;      
    int responce_cache_len;

    char request_frame_no;
    char responce_frame_no;
    
} frame_sender;

/* data_received, uploaded,  */
typedef struct received_data_{
    // data_node* data_uploaded;
    // data_node* data_uploaded_quick;
    // data_node* data_uploaded_block;

    char* data_requested;
} received_data;

/* handshake */
typedef struct handshake_{
   bool bdown;
   bool bup;
   timer_t timer_poweron_frame;
   int cnt_poweron_frame;
   timer_t timer_handshake_up;
   timer_t timer_handshake_down;
} handshake;

/* link error */
typedef struct link_error_{
  timer_t timer_link_error_frame;
} link_error;



#ifdef USE_STATISTICS_MODULE
/* statistics module */
typedef struct statistics_module_{
    
    int cnt_fto;                /* frame time out count */
    int cnt_frame_received;     /* frame received count */
    int cnt_dropped_sframe;     /* dropped short frame count */
    int cnt_checksum_eframe;    /* checksum error frame count */
    int cnt_dropped_eframe;     /* dropped checksum error ctrl count */
    int cnt_perfect_frame;       /* received perfect frame count */
    int cnt_resend_cmd_frame;    /* notify resend frame count */

    int cnt_frame_error_write;	/* frame error writed count */ 
    int cnt_frame_writed;       /* frame writed count */ 
    int cnt_frame_resended;     /* frame resended count */

} statistics_module;
#endif

/* protocal core struct */
typedef struct protocal_struct_ {
    int fd;	                       /* serial port fd */

    pthread_t thr_handle_request;   /* thread which handles request from top layer */
    pthread_t thr_read_serial;      /* thread for read serial port */
    pthread_t thr_data_send;        /* thread which send data to top layer */  

    frame_receiver stframe_receiver;    
    frame_sender   stframe_sender;

    handshake st_hs;
    link_error st_le;

    protocal_state pro_state;
#ifdef USE_STATISTICS_MODULE
    statistics_module stat;
#endif
} protocal_struct;

/* core protocal */
protocal_struct pro_st; 

static char childboard_cmds[] = {
	FRAME_CMD_CHILDBOARD_POWER_ON,
	FRAME_CMD_CHILDBOARD_LINK_DETECT,
	FRAME_CMD_CHILDBOARD_IC_CONFIRM,
	FRAME_CMD_CHILDBOARD_IC_READ_RESULT,
	FRAME_CMD_CHILDBOARD_IC_PULLOUT_NOTIFY,
	FRAME_CMD_CHILDBOARD_VARIABLE_CONTINUS,
	FRAME_CMD_CHILDBOARD_VARIABLE_BOOL,
	FRAME_CMD_CHILDBOARD_VARIABLE_CHAR,
	FRAME_CMD_CHILDBOARD_VARIABLE_BLOCK,
	FRAME_CMD_CHILDBOARD_TO_SERVER
};

static char mainboard_cmds[] = {	
	FRAME_CMD_MAINBOARD_POWER_MANAGE,
	FRAME_CMD_MAINBOARD_VERSION,
	FRAME_CMD_MAINBOARD_CHECK,
	FRAME_CMD_MAINBOARD_FIRMWARE_UPDATE,
	FRAME_CMD_MAINBOARD_DEVICE_ATTR,
	FRAME_CMD_MAINBOARD_READ_IC,
	FRAME_CMD_MAINBOARD_POWER_ON,
	FRAME_CMD_MAINBOARD_CONFIG_QUERY,
	FRAME_CMD_MAINBOARD_QUERY,
	FRAME_CMD_MAINBOARD_CTRL,
	FRAME_CMD_MAINBOARD_LOCATION,
	FRAME_CMD_MAINBOARD_CONFIG,
	FRAME_CMD_MAINBOARD_SERVER_DATA,
	FRAME_CMD_MAINBOARD_CFIRMWARE_UPDATE
};

int create_protocal_threads();
int frame_general_create(char cmd, void* userdata, int datalen, char** ppframe);
int frame_general_create_with_nocheck(char cmd, void* userdata, int datalen, char** ppframe);
bool is_valid_cmd(char cmd);
void frame_write(char* pframe, int len, bool bcached);
void frame_send_poweron_frame();
void timer_frame_poweron_timeout();
char frame_get_cmd_checksum_passed(char* pframe);
char frame_get_cmd_unchecksumed(char* pframe);
int frame_get_checksum_start_pos(char* pframe, int len);
bool frame_checksum(char* pframe, int len);
int initProtocal(void){
   int thread_err;

   /* init serial port */
   int fd = initSerial();
   if (fd < 0){
	return fd;
   }
   pro_st.fd = fd;

   /* init state */
   pro_st.pro_state = E_PSTATE_INIT;	
   PRO_PRINTFH("===== STATE CHANGE TO : INIT =====\n");

   /* start protocal ralatied thread */
   thread_err = create_protocal_threads();
   if (thread_err < 0){
       goto err1;
   }

   return 0;
	

err1:
   close_port(fd);	
   return -1;
}

void finalizeProtocal(void){


   close_port(pro_st.fd);
}

int getData(int cmd, void* data, int length){
    return 0;
}

void *thread_handle_request(void *arg){
   char* p = NULL;
   char a[] = {0x11, 0x22, 0x33, 0x7E, 0x7E, 0x66, 0x7D, 0x7D, 0x99};
   int len = 0;

   PRO_PRINTFH("thread_handle_request runs\n"); 
   /*
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,a,9,&p);

   PRO_PRINTF("frame len:%d\n", len);
   {
      int i = 0;
      for (i = 0; i < len; i++)
      {
          PRO_PRINTF("0x%02hhX,", *(p+i));
      }
      PRO_PRINTF("\n");
      write_data(pro_st.fd, p, len);
   }

   char b[256];
   int j=0;
   for (j=0; j<256; j++){
      b[j] = j;
   }
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,b,256,&p);
   PRO_PRINTF("frame len:%d\n", len);
   {
      int i = 0;
      for (i = 0; i < len; i++)
      {
          PRO_PRINTF("0x%02hhX,", *(p+i));
      }
      PRO_PRINTF("\n");
   }
   write_data(pro_st.fd, p, len); 
   */
   /*
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,NULL,0,&p);
   write_data(pro_st.fd, p, len);
   len = frame_general_create(FRAME_CMD_MAINBOARD_POWER_MANAGE,NULL,0,&p);
   write_data(pro_st.fd, p, len);*/

   char bufa[100];
   /*
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,NULL,0,&p);
   memcpy(bufa, p, 5);
   write_data(pro_st.fd, bufa, 5);
   sleep(1);
 
   memcpy(bufa, p+5, len -5);
   len = frame_general_create(FRAME_CMD_MAINBOARD_POWER_MANAGE,NULL,0,&p);
   memcpy(bufa+(len -5), p, len);
   write_data(pro_st.fd, bufa, len+5); */

   /* test of error ctrl */	
   #if 0
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,NULL,0,&p);
   *(p+1) = 0xAA;
   frame_write(p, len ,true);
   #endif
   /*
   len = frame_general_create(FRAME_CMD_ERROR_FRAME,NULL,0,&p);
   *(p+1) = 0xAA;
   frame_write(p, len ,true);*/
   #if 0
   sleep(1);
   PRO_PRINTF("child borard1:\n");
   len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,NULL,0,&p);
   frame_write(p, len ,false); 

   char cc = frame_get_cmd_checksum_passed(p);
            PRO_PRINTF("if-cmd:0x%02hhX\n", cc);
   cc = frame_get_cmd_unchecksumed(p);
            PRO_PRINTF("if-cmd:0x%02hhX\n", cc);
   PRO_PRINTF("child borard2:\n");
   char aaa = (char)DEVICE_CHILDBOARD;
   len = frame_general_create((FRAME_CMD_MAINBOARD_CHECK),&aaa,1,&p);
   frame_write(p, len ,false); 
   cc = frame_get_cmd_checksum_passed(p);
            PRO_PRINTF("if-cmd:0x%02hhX\n", cc);
   cc = frame_get_cmd_unchecksumed(p);
            PRO_PRINTF("if-cmd:0x%02hhX\n", cc);
   #endif

   return NULL;
}  

int frame_cache_type(char cmd){
    int cache_type = -1;
    int i = 0;

    
    for (i = 0; i < (int)(sizeof(childboard_cmds)/sizeof(childboard_cmds[0])); i++)
     {
	if ((char)childboard_cmds[i] == (char)cmd){
	    cache_type = 1;  // responce cmd
	    break;
	}
     }

    if (cache_type >= 0){
       PRO_PRINTF("frame_cache_type: %d, cmd:0x%02hhX\n", 1, cmd);
       return cache_type;
     }

    if (cmd == (char) FRAME_CMD_ERROR_CTRL){
       PRO_PRINTF("frame_cache_type: %d, cmd:0x%02hhX\n", 1, cmd);
         return 1;
     }

    for (i = 0; i < (int)(sizeof(mainboard_cmds)/sizeof(mainboard_cmds[0])); i++)
    {
	if (mainboard_cmds[i] == cmd){

	    cache_type = 0;
	    break;
	}
    }

    PRO_PRINTF("frame_cache_type: %d, cmd:0x%02hhX\n", cache_type, cmd);
    return cache_type;	
}

void frame_write(char* pframe, int len, bool bcache){
   frame_sender* pfs = &(pro_st.stframe_sender);   
   int len_write = write_data(pro_st.fd, pframe, len);
   PRO_BLOCKPRINTF("frame_write:", pframe , len);
   if (len_write < len){
       PRO_PRINTFH("fatal error--- frame write error happends!\n"); 
   }

   // cache the frame;
   if (bcache){
       char cmd = frame_get_cmd_checksum_passed(pframe);
       int cache_type = frame_cache_type(cmd);
       if (cache_type == 0){
           // request cache
           if(NULL != pfs->request_cache){
                   free(pfs->request_cache);
              }
           pfs->request_cache = pframe;
           pfs->request_cache_len = len;
       } else if (cache_type == 1) {
             // responce cache
           if(NULL != pfs->responce_cache){
                   free(pfs->responce_cache);
             }
           pfs->responce_cache = pframe;
           pfs->responce_cache_len = len;
         }
   } 

#ifdef USE_STATISTICS_MODULE
   statistics_module * pstat = &(pro_st.stat);   
   if (len_write < len){
       pstat->cnt_frame_error_write++; 
   }else{
       pstat->cnt_frame_writed++; 
   } 

   if (len_write == len && !bcache){
       pstat->cnt_frame_resended++; 
   }
#endif
}

void frame_resend(char cmd){

    frame_sender* pfs = &(pro_st.stframe_sender);
      

    int j=0;
    int start_pos;
     char* pframe = NULL;
    int len = 0;
    PRO_PRINTF("frame_resend, cmd:0x%02hhX", cmd);
    int cache_type = frame_cache_type(cmd);
    if (cache_type == 0){
           // request cache
           if(NULL != pfs->request_cache){
                 if (cmd == frame_get_cmd_unchecksumed(pfs->request_cache)){
                       pframe = pfs->request_cache;
                       len = pfs->request_cache_len;
                     }
              }

    } else if (cache_type == 1){
             // responce cache
           if(NULL != pfs->responce_cache){
                 if (cmd == frame_get_cmd_unchecksumed(pfs->responce_cache)){
                       pframe = pfs->responce_cache;
                       len = pfs->responce_cache_len;
                     }
             }
     }     

     if (NULL != pframe){
            /* re-generate checksum */
             start_pos = frame_get_checksum_start_pos(pframe, len);

            char check_sum=0;


           for (j = start_pos; j <len-1; j++){
               check_sum += *(pframe + j);
             }
           if (check_sum == FRAME_FLAG || check_sum == (FRAME_FLAG-1)){
                if(start_pos != 5){
		       PRO_PRINTFH("fatal error: unable to rechecksum,1 frame checksum start_pos:%d\n", start_pos);
                    }
                *(pframe+1) = (FRAME_FLAG - 1);
                *(pframe+2) = (2- (FRAME_FLAG - check_sum));
           }else{
                if (start_pos != 4){
                        PRO_PRINTFH("fatal error: unable to rechecksum,2 frame checksum start_pos:%d\n", start_pos);
                   } 
		   *(pframe + 1) = check_sum;
              }
            
            /* resend, but not cache the frame */
            frame_write(pframe, len, false);

        }      

}

void frame_read(){
    char buf[FRAME_RECEIVER_BUF_LEN+2];
    frame_receiver* pfr = &(pro_st.stframe_receiver);


    if (pfr->frame_time_out){
        // drop the data 
        pfr->frame_time_out = false;
        pfr->pos = 0;
     }

    int len=read_data(pro_st.fd, buf, FRAME_RECEIVER_BUF_LEN);
    PRO_BLOCKPRINTF("readed data:", buf, len);
        
    int i = 0;

    if (pfr->bincomplete_frame){
        PRO_PRINTF("bincomplete_frame:true\n"); 
        for (i=0; i<len; i++){
            *(pfr->accept_buf + pfr->pos++) = *(buf+i);
            if (*(buf+i) == FRAME_FLAG){
                pfr->frame_flag_count++;

                if (pfr->frame_flag_count == 1 || pfr->frame_flag_count == 3){
                    pfr->bincomplete_frame = false;

                        {
                           /* cancel syn frame timer */
                        pro_timer_cancel(pfr->timer_frame_syn);

                        if (!pfr->bbuf1){
		                   memcpy(pfr->buf1, pfr->accept_buf, pfr->pos);
		                   pfr->buf1_datalen = pfr->pos;
		                   pfr->bbuf1 =  true;
                        } else if (!pfr->bbuf2){
		                   memcpy(pfr->buf2, pfr->accept_buf, pfr->pos);
		                   pfr->buf2_datalen = pfr->pos;
		                   pfr->bbuf2 =  true;                           
                        } else {
		                   PRO_PRINTFH("critical error: accepter overflow!\n");
                        }  
		               
                        pfr->pos = 0;
                        memset(pfr->accept_buf, 0, FRAME_RECEIVER_BUF_LEN+2);
		   }
               } else if (pfr->frame_flag_count == 2){
                       pfr->bincomplete_frame = true; 
               } else {
                      PRO_PRINTFH("critical error: too many frames!\n");
               }
          }  
       }
    }else{
             PRO_PRINTF("bincomplete_frame:false\n"); 
             for (i=0; i<len; i++){
                *(pfr->accept_buf + pfr->pos++) = *(buf+i);
                if (*(buf+i) == FRAME_FLAG){
                    pfr->frame_flag_count++;
                    
                    if (pfr->frame_flag_count == 1){
		       pfr->bincomplete_frame = true;
                    } else if (pfr->frame_flag_count == 2 || pfr->frame_flag_count == 4){
                       pfr->bincomplete_frame = false;
                       if (!pfr->bbuf1){
                           memcpy(pfr->buf1, pfr->accept_buf, pfr->pos);
                           pfr->buf1_datalen = pfr->pos;
                           pfr->bbuf1 =  true;
                       } else if (!pfr->bbuf2){
                           memcpy(pfr->buf2, pfr->accept_buf, pfr->pos);
                           pfr->buf2_datalen = pfr->pos;
                           pfr->bbuf2 =  true;                           
                       } else {
                           PRO_PRINTFH("critical error: accepter overflow!\n");
                       }  
                       
                       pfr->pos = 0;
                       memset(pfr->accept_buf, 0, FRAME_RECEIVER_BUF_LEN+2);
                   } else if (pfr->frame_flag_count == 3){
                       pfr->bincomplete_frame = true; 
                   } else {
                      PRO_PRINTFH("critical error: too many frames!\n");
                   }
                }
            } 
        }

    if (pfr->bbuf1){
        PRO_BLOCKPRINTF("frame1 readed:", pfr->buf1, pfr->buf1_datalen);
    } 
    if (pfr->bbuf2){
	PRO_BLOCKPRINTF("frame2 readed:", pfr->buf2, pfr->buf2_datalen);
    }
}

void timer_frame_syn_callback(){
    PRO_PRINTFH("timer_frame_syn_callback\n");
    frame_receiver* pfr = &(pro_st.stframe_receiver);
    pfr->frame_time_out = true;

#ifdef USE_STATISTICS_MODULE
    statistics_module * pstat = &(pro_st.stat);
    pstat->cnt_fto ++;
#endif
}

void frame_syn(){

    frame_receiver* pfr = &(pro_st.stframe_receiver);
    if (pfr->bincomplete_frame && 1 != pro_timer_is_running(pfr->timer_frame_syn)) {
    	pro_timer_create(&(pfr->timer_frame_syn), timer_frame_syn_callback, PRO_TIMER_SYN_FRAME);
    }
#ifdef USE_STATISTICS_MODULE
   statistics_module * pstat = &(pro_st.stat);
    if (pfr->bbuf1)
        pstat->cnt_frame_received++;
    if (pfr->bbuf2)
        pstat->cnt_frame_received++;    
#endif    
    if (pfr->bbuf1 && pfr->buf1_datalen < 9) {
        // drop the short frame
        pfr->bbuf1 = false;
        pfr->buf1_datalen = 0;
        PRO_PRINTFH("drop 1 short frame\n");
#ifdef USE_STATISTICS_MODULE
        pstat->cnt_dropped_sframe ++;
#endif
    }
    
    if (pfr->bbuf2 && pfr->buf2_datalen < 9) {
        // drop the short frame
        pfr->bbuf2 = false;
        pfr->buf2_datalen = 0;
        PRO_PRINTFH("drop 1 short frame");
#ifdef USE_STATISTICS_MODULE
        pstat->cnt_dropped_sframe ++;
#endif
    }
}

void reset_frame_bufs(){
    
    frame_receiver* pfr = &(pro_st.stframe_receiver);
    
    pfr->buf1_datalen = 0;
    pfr->bbuf1 = false;
    pfr->buf2_datalen = 0;
    pfr->bbuf2 = false;
    pfr->frame_flag_count = 0;
}

int frame_get_checksum_start_pos(char* pframe, int len){
        int i = 0;
        int j = 0;

        bool bvp = false;
        char vp[4];
         int pos = 0;
        short stemp = FRAME_VERSION;            // version
        vp[0] = (char)((stemp & 0xFF00) >> 8);
        vp[1] = (char)(stemp & 0x00FF);
    
        stemp = FRAME_PRODUCER;                 // producer
        vp[2] = (char)((stemp & 0xFF00) >> 8);
        vp[3] = (char)(stemp & 0x00FF);
        for (i=2; i<4 ;i++){
            for (j=0; j<4; j++){
                  if( *(pframe+i+j) != vp[j] ){
                            break;
                      }
                  if (j == 3) 
 		           bvp = true;
               } 
            if (bvp) {
                 pos = i;
                    break;
               }
          }
          pos += 2;
          return pos;
}

bool frame_checksum(char* pframe, int len){

        char check_sum = 0;
        int i = 0;
        int j = 0;
        char checksum_old;
          
        /* 1, check the version and producer */
        bool bvp = false;
        char vp[4];
        short stemp = FRAME_VERSION;            // version
        vp[0] = (char)((stemp & 0xFF00) >> 8);
        vp[1] = (char)(stemp & 0x00FF);
    
        stemp = FRAME_PRODUCER;                 // producer
        vp[2] = (char)((stemp & 0xFF00) >> 8);
        vp[3] = (char)(stemp & 0x00FF);
        for (i=2; i<4 ;i++){
            for (j=0; j<4; j++){
                  if( *(pframe+i+j) != vp[j] ){
                            break;
                      }
                  if (j == 3) 
 		           bvp = true;
               } 
            if (bvp) {
                    break;
               }
          }
          
        if (!bvp)
             return false;

        /* 2, checksum */
        if ( *(pframe + 1) == (char)(FRAME_FLAG - 1) ){
        	for (i = 5; i <len-1; i++){
        	   check_sum += *(pframe + i);
          	}
             checksum_old = *(pframe + 2) + *(pframe + 3) - 1;          
        }else{
        	for (i = 4; i <len-1; i++){
        	   check_sum += *(pframe + i);
          	}
             checksum_old = *(pframe + 1);          
          }

	 if (*(pframe + 1) == check_sum){
            return true; 
         }

       return false;    
}

char frame_get_cmd_checksum_passed(char* pframe){
      char cmd = *(pframe + 7);
      if ( *(pframe + 1) == (char)(FRAME_FLAG - 1) ){
          cmd = *(pframe + 8);
       }  
       return cmd;
}
char frame_get_cmd_need_resend(char* pframe){
      char cmd = *(pframe + 8);
      if ( *(pframe + 1) == (char)(FRAME_FLAG - 1) ){
          cmd = *(pframe + 9);
       }  
       return cmd;
}
char frame_get_cmd_unchecksumed(char* pframe){
        
        char cmd = 0;

        // try to find the version and producer
         
        int i = 0;
        int j = 0;

        bool bvp = false;
        char vp[4];
         int pos = 0;
        short stemp = FRAME_VERSION;            // version
        vp[0] = (char)((stemp & 0xFF00) >> 8);
        vp[1] = (char)(stemp & 0x00FF);
    
        stemp = FRAME_PRODUCER;                 // producer
        vp[2] = (char)((stemp & 0xFF00) >> 8);
        vp[3] = (char)(stemp & 0x00FF);
        for (i=2; i<4 ;i++){
            for (j=0; j<4; j++){
                  if( *(pframe+i+j) != vp[j] ){
                            break;
                      }
                  if (j == 3) 
 		           bvp = true;
               } 
            if (bvp) {
                 pos = i;
                    break;
               }
          }
          
        if (bvp){
             cmd = *(pframe + 5 + pos);
        } else {
             cmd = *(pframe + 7);
          }
        return cmd;
}

void frame_error_process(char* pinframe, int* plen, bool* pb){
#ifdef USE_STATISTICS_MODULE
    statistics_module * pstat = &(pro_st.stat);
#endif 
        if (frame_checksum(pinframe, *plen)){
            // checksum passed
            char cmd = frame_get_cmd_checksum_passed(pinframe);
            PRO_PRINTF("if-cmd:0x%02hhX\n", cmd);
            if (cmd == (char)FRAME_CMD_ERROR_FRAME){
                /* resend cmd received, resend the frame */
#ifdef USE_STATISTICS_MODULE
                pstat->cnt_resend_cmd_frame++;
#endif	   
                char cmd_to_resend = frame_get_cmd_need_resend(pinframe);
                PRO_PRINTFH("frame_error_ctrl: resend cmd received,cmd:0x%02hhX\n",cmd_to_resend);	
                frame_resend(cmd_to_resend);

                /* drop the receive cmd frame after send */
                *pb = false;
                *plen = 0;

            }else{
                /* perfect frames, wait to further process */
                PRO_PRINTF("frame_error_ctrl: perfect frame received.\n");
#ifdef USE_STATISTICS_MODULE
                pstat->cnt_perfect_frame++;
#endif
            }
        }else{
            // checksum failed
#ifdef USE_STATISTICS_MODULE
            pstat->cnt_checksum_eframe ++;
#endif                 
            char cmd = frame_get_cmd_unchecksumed(pinframe);
            PRO_PRINTF("else-cmd:0x%02hhX\n", cmd);
            if (cmd == (char)FRAME_CMD_ERROR_FRAME){
                // drop the frame
                *pb = false;
                *plen = 0;
                PRO_PRINTFH("frame_error_ctrl: drop 1  checksum failed error ctrl frame\n");
#ifdef USE_STATISTICS_MODULE
                pstat->cnt_dropped_eframe++;
#endif                 
            }else{
                char* pframe;
                PRO_PRINTFH("frame_error_ctrl: checksum failed need to re-request the frame\n");
		   int frame_len = frame_general_create_with_nocheck(FRAME_CMD_ERROR_FRAME, &cmd, 1, &pframe);

		   if (frame_len >= 9){
                    frame_write(pframe, frame_len, false); 
                                 
                }else{
                    PRO_PRINTFH("fatal error : frame_error_ctrl: frame error created,error:%d\n, cmd:0x%02hhX\n", frame_len, cmd);
		    }  
                *pb = false;
                *plen = 0;  
	        }
        }
}

void frame_error_ctrl(){
    frame_receiver* pfr = &(pro_st.stframe_receiver);

    if (pfr->bbuf1){
        frame_error_process(pfr->buf1, &pfr->buf1_datalen, &pfr->bbuf1);
    }
    if (pfr->bbuf2){
        frame_error_process(pfr->buf2, &pfr->buf2_datalen, &pfr->bbuf2);
    }
}

void frame_send_poweron_frame(){

    handshake* phs = &(pro_st.st_hs);
    PRO_PRINTFH("frame_send_poweron_frame, no response times:%d\n", phs->cnt_poweron_frame);
    char* pframe_poweron;
    int len = frame_general_create((char)FRAME_CMD_MAINBOARD_POWER_ON,NULL,0,&pframe_poweron);
    frame_write(pframe_poweron, len ,true);
    
}

void frame_send_poweron_frame_with_timer(){
    handshake* phs = &(pro_st.st_hs);    
    PRO_PRINTFH("frame_send_poweron_frame_with_timer, no response times:%d\n", phs->cnt_poweron_frame);
    frame_send_poweron_frame();

 
    pro_timer_create(&(phs->timer_poweron_frame), 
       timer_frame_poweron_timeout, PRO_TIMER_POWERON_FRAME_TIMEOUT);
}

void frame_send_linkerror_poweron_frame(){
    PRO_PRINTFH("frame_send_linkerror_poweron_frame");
    char* pframe_poweron;
    int len = frame_general_create(FRAME_CMD_MAINBOARD_POWER_ON,NULL,0,&pframe_poweron);
    frame_write(pframe_poweron, len ,true);
}

void timer_link_error_poweron_frame_timeout(){
    PRO_PRINTFH("timer_link_error_poweron_frame_timeout\n");
    frame_send_linkerror_poweron_frame();
}

void timer_frame_poweron_timeout(){
    PRO_PRINTFH("timer_frame_poweron_timeout\n");
    handshake* phs = &(pro_st.st_hs);
    phs->cnt_poweron_frame++;
    if (phs->cnt_poweron_frame > 9){
        pro_st.pro_state = E_PSTATE_LINKERROR;
	PRO_PRINTFH("=====STATE CHANGE TO : LINK ERROR =====\n");

        pro_timer_cancel(phs->timer_poweron_frame);
        
        /* TODO:send link error to server */
        
        /* send link error power on frame */
        phs->cnt_poweron_frame = 0;
        link_error* ple = &(pro_st.st_le);

        frame_send_linkerror_poweron_frame();
        pro_timer_create(&(ple->timer_link_error_frame), 
           timer_link_error_poweron_frame_timeout, PRO_TIMER_LERROR_POWERON_FRAME); 
    } else {
        frame_send_poweron_frame();
    }
}

void timer_frame_handshake_down_timeout(){
    PRO_PRINTFH("timer_frame_handshake_timeout\n");
    handshake* phs = &(pro_st.st_hs);
    phs->bdown = false; 

    frame_send_poweron_frame_with_timer();
}

void timer_frame_handshake_up_timeout(){
    PRO_PRINTFH("timer_frame_handshake_up_timeout\n");
    handshake* phs = &(pro_st.st_hs);
    phs->bup = false; 
}

void frame_responce_poweroncmd(char* pframe, int* plen, bool* pb){
    handshake *phs = &(pro_st.st_hs);
    char cmd = frame_get_cmd_checksum_passed(pframe);
    PRO_PRINTF("frame_responce_poweroncmd cmd:0x%02hhX\n", cmd); 

    switch (cmd) {
        case (char)FRAME_CMD_MAINBOARD_POWER_ON :
        {
            
            PRO_PRINTF("frame_responce_poweroncmd cmd1:0x%02hhX\n", cmd); 
            if (pro_st.pro_state == E_PSTATE_HANDSHAKE){ 
		    phs->bdown = true;
		    phs->cnt_poweron_frame = 0;
		    pro_timer_cancel(phs->timer_poweron_frame);


		    pro_timer_create(&phs->timer_handshake_down, timer_frame_handshake_down_timeout, PRO_TIMER_HANDSHAKE_DOWN);
		    if (phs->bup){
		            /* handshake finish */ 
       		            pro_timer_cancel(phs->timer_handshake_down);
       		            pro_timer_cancel(phs->timer_handshake_up);
		            pro_st.pro_state = E_PSTATE_COMMUNICATE;
		            PRO_PRINTFH("=============================\n");
		            PRO_PRINTFH("===== handshake finish! =====\n");
		            PRO_PRINTFH("=============================\n");
	    	            PRO_PRINTFH("===== STATE CHANGE TO : COMMUNICATE =====\n");

		            phs->bup = false;
		            phs->bdown = false;                     
		    }
          
            } else if (pro_st.pro_state == E_PSTATE_LINKERROR){
	 	pro_st.pro_state = E_PSTATE_HANDSHAKE;
                PRO_PRINTFH("===== STATE CHANGE TO : HANDSHAKE =====\n");
                phs->bdown = true;
                link_error* ple = &(pro_st.st_le);
                pro_timer_cancel(ple->timer_link_error_frame);

                pro_timer_create(&phs->timer_handshake_down, timer_frame_handshake_down_timeout, PRO_TIMER_HANDSHAKE_DOWN);
            }
            *pb = false;
            *plen = 0;
            break; 
        }
        case (char)FRAME_CMD_CHILDBOARD_POWER_ON :
        {
            /* response */
            if (pro_st.pro_state != E_PSTATE_LINKERROR) {
                char* p;
                int len = frame_general_create(FRAME_CMD_CHILDBOARD_POWER_ON,NULL,0,&p);
                frame_write(p, len ,true);
            }
            phs->bup = true;
            pro_timer_create(&phs->timer_handshake_up, timer_frame_handshake_up_timeout, PRO_TIMER_HANDSHAKE_UP);
            if (phs->bdown && pro_st.pro_state == E_PSTATE_HANDSHAKE){
                /* handshake finish */ 
                pro_st.pro_state = E_PSTATE_COMMUNICATE;
       		pro_timer_cancel(phs->timer_handshake_down);
       		pro_timer_cancel(phs->timer_handshake_up);
                PRO_PRINTFH("=============================\n");
                PRO_PRINTFH("===== handshake finish! =====\n");
                PRO_PRINTFH("=============================\n");
      	        PRO_PRINTFH("===== STATE CHANGE TO : COMMUNICATE =====\n");
                phs->bup = false;
                phs->bdown = false;
            }else if (pro_st.pro_state == E_PSTATE_COMMUNICATE && !phs->bdown){
      	        PRO_PRINTFH("===== STATE CHANGE TO : HANDSHAKE =====\n");
                pro_st.pro_state = E_PSTATE_HANDSHAKE;
		frame_send_poweron_frame_with_timer();
            }
            *pb = false;
            *plen = 0;

            break;
        }
        default:
            break;
    }     
}

void frame_responce_common_cmd(char* pframe, int* plen, bool* pb){
    handshake *phs = &(pro_st.st_hs);
    char cmd = frame_get_cmd_checksum_passed(pframe);
    PRO_PRINTF("frame_responce_common_cmd cmd:0x%02hhX\n", cmd); 

    char* pframe_responce = NULL;  
     int  len = 0;
    /* responce unkown cmds */   
    bool bvalid = is_valid_cmd(cmd);
    if (!bvalid && cmd != (char)(FRAME_CMD_ERROR_CTRL)){     
    	int len = frame_general_create((char)FRAME_CMD_ERROR_CTRL,&cmd,1,&pframe_responce);
    	frame_write(pframe_responce, len ,true);
            *pb = false;
            *plen = 0;
     } 
    if (cmd == (char)(FRAME_CMD_ERROR_CTRL))     {
        *pb = false;
        *plen = 0;
     }
    /* responce childboard cmds */
    switch (cmd) {
       case (char)FRAME_CMD_CHILDBOARD_LINK_DETECT :
       case (char)FRAME_CMD_CHILDBOARD_IC_READ_RESULT :  // TODO:
       case (char)FRAME_CMD_CHILDBOARD_IC_PULLOUT_NOTIFY:  // TODO:
       {      
            len = frame_general_create(cmd,NULL,0,&pframe_responce);
    	      frame_write(pframe_responce, len ,true);
            break; 
       }
      case (char) FRAME_CMD_CHILDBOARD_IC_CONFIRM:  // TODO:
       {
             break;
        }
      case (char) FRAME_CMD_CHILDBOARD_VARIABLE_CONTINUS:
      case (char) FRAME_CMD_CHILDBOARD_VARIABLE_BOOL:
      case (char) FRAME_CMD_CHILDBOARD_VARIABLE_CHAR:
        {  
            int data_pos = 8;
            if (*(pframe  +7) != cmd){
                data_pos++;
               }
            char data_responce[6];
            data_responce[0] = *(pframe+data_pos++);  
            data_responce[1] = *(pframe+data_pos++);
            data_responce[2] = *(pframe+data_pos++);
            data_responce[3] = *(pframe+data_pos++);
            data_responce[4] = *(pframe+data_pos);
            data_responce[5] = 0;
            len = frame_general_create(cmd, data_responce, 6 ,&pframe_responce);
  	      frame_write(pframe_responce, len ,true);
            break;
        }
      case (char) FRAME_CMD_CHILDBOARD_VARIABLE_BLOCK:
        {
            int data_pos = 8;
            if (*(pframe  +7) != cmd){
                data_pos++;
               }
            char data_responce[12];
            int i = 0;
            for (i=0; i<11;i++){
               data_responce[i] = *(pframe+data_pos+i);  
               }
            data_responce[11] = 0;

            len = frame_general_create(cmd, data_responce, 12 ,&pframe_responce);
  	      frame_write(pframe_responce, len ,true);
            break;
        }
       case  (char)FRAME_CMD_CHILDBOARD_TO_SERVER:
        {
            int data_pos = 8;
            if (*(pframe  + 7) != cmd){
                data_pos++;
               }
            char data_responce[2];
            data_responce[0] = *(pframe+data_pos);  
            data_responce[1] = 0;

            len = frame_general_create(cmd, data_responce, 2 ,&pframe_responce);
  	      frame_write(pframe_responce, len ,true);
            break;
        }
       default:
            break;
    }     
}

void frame_responce(){
    PRO_PRINTF("frame_responce\n"); 
    frame_receiver* pfr = &(pro_st.stframe_receiver);
    
    if (pfr->bbuf1){
        frame_responce_poweroncmd(pfr->buf1, &pfr->buf1_datalen, &pfr->bbuf1);
     }
    if (pfr->bbuf2){
        frame_responce_poweroncmd(pfr->buf2, &pfr->buf2_datalen, &pfr->bbuf2);
     }

    if (pro_st.pro_state == E_PSTATE_LINKERROR){
         return;
     } 
     
    if (pfr->bbuf1){
       frame_responce_common_cmd(pfr->buf1, &pfr->buf1_datalen, &pfr->bbuf1);
     } 
    if (pfr->bbuf2){
       frame_responce_common_cmd(pfr->buf2, &pfr->buf2_datalen, &pfr->bbuf2);
     } 
}

void *thread_read_serial(void *arg){
    PRO_PRINTFH("thread_read_serial runs\n"); 

     

    /* send power on frame */
    pro_st.pro_state = E_PSTATE_HANDSHAKE;
    PRO_PRINTFH("===== STATE CHANGE TO : HANDSHAKE =====\n");
    frame_send_poweron_frame_with_timer();

    /* start read loop */
    frame_receiver* pfr = &(pro_st.stframe_receiver);
    while (1){
       /* read frame */ 
       frame_read();
       /* frame synchronous, if there are incomleted frame */
       frame_syn();       
       /* frame error control */
       frame_error_ctrl();     
       /* do responce */
       frame_responce();
       /* parse frame data and store */ 
         
       /* reset frame buffers */
       reset_frame_bufs();
    }
}  

void *thread_data_send(void *arg){
   PRO_PRINTFH("thread_data_send runs\n"); 
   return NULL;
}  





int create_protocal_threads(){
   /* start request handle thread */
   int err = pthread_create(&pro_st.thr_handle_request, NULL, thread_handle_request,NULL); 
   if (err != 0){
        PRO_PRINTF("can't create thread_handle_request thread");
        goto err1;
   }

   /* start main read thread */
   err = pthread_create(&pro_st.thr_read_serial, NULL, thread_read_serial, NULL); 
   if (err != 0){
        PRO_PRINTF("can't create thread_read_serial thread");
        goto err2;
   }

   /* start data send thread */
   err = pthread_create(&pro_st.thr_data_send, NULL, thread_data_send,NULL); 
   if (err != 0){
        PRO_PRINTF("can't create thread_data_send thread");
        goto err3;
   }

   return 0;

err3:
  		
err2:
   	
err1:
   return -1;
}

int frame_general_create_with_nocheck(char cmd, void* userdata, int datalen, char** ppframe){
    
    char* indata = (char*)userdata;
    char* pframe = NULL;
    int frame_len = 0;
    int frame_special_char_cnt = 0;    
    int i=0;
    int frame_pos = 0;
    short stemp;

  
    if (NULL == userdata && datalen > 0){

       return -2;
     }
    if(0 == datalen && userdata != NULL){

        return -3;
     }

    /* malloc memory for frame */
    for (i=0; i<datalen; i++){
        if (*(indata+i) == FRAME_FLAG || *(indata+i) == (FRAME_FLAG-1))
            frame_special_char_cnt++;
    }

    frame_len = FRAME_NUSERDATA_LEN + frame_special_char_cnt + datalen + 1;  

    pframe = (char*)malloc(frame_len);
    if (NULL == pframe)
        return -4;
    memset(pframe, 0, frame_len);    

    /* package a frame */
    *(pframe + frame_pos) = FRAME_FLAG;     // flag
    frame_pos += 2;
                  
    stemp = FRAME_VERSION;                  // version
    *(pframe + frame_pos++) = (char)((stemp & 0xFF00) >> 8);
    *(pframe + frame_pos++) = (char)(stemp & 0x00FF);
    
    stemp = FRAME_PRODUCER;                 // producer
    *(pframe + frame_pos++) = (char)((stemp & 0xFF00) >> 8);
    *(pframe + frame_pos++) = (char)(stemp & 0x00FF);
    
    *(pframe + frame_pos++) = (char)FRAME_DEVICE; // device
    *(pframe + frame_pos++) = (char)cmd;          // commands
    
    for (i=0; i<datalen; i++){              
        if (*(indata+i) ==  FRAME_FLAG){
            *(pframe + frame_pos++) = FRAME_FLAG - 1;
	    *(pframe + frame_pos++) = 0x02;
        }
        else if(*(indata+i) ==  FRAME_FLAG - 1){
	    *(pframe + frame_pos++) = FRAME_FLAG - 1;
	    *(pframe + frame_pos++) = 0x01;
	}
        else{
            *(pframe + frame_pos++) = *(indata + i);
        }
    }
    *(pframe + frame_pos) = FRAME_FLAG;     // end flag
    
   {   // create checksum
        char check_sum = 0;
        int i = 0;
        for (i = 0; i < 4 + frame_special_char_cnt + datalen; i++){
        	    check_sum += *(pframe + 4 + i);
          }
        if (check_sum == FRAME_FLAG || check_sum == (FRAME_FLAG-1)){
                for (i=frame_len-1; i>=3; i--){
                    	*(pframe+i) = *(pframe+i-1);
                    }
                       
                *(pframe+1) = (FRAME_FLAG - 1);
                *(pframe+2) = (2- (FRAME_FLAG - check_sum));
        }else{
		   *(pframe + 1) = check_sum;
                frame_len--;
          }
	
    }    

    *ppframe = pframe;
    return frame_len;
}

int frame_general_create(char cmd, void* userdata, int datalen, char** ppframe){
   /* check input para */
   if(!is_valid_cmd(cmd)){
        return -1;
    }
   return frame_general_create_with_nocheck(cmd, userdata, datalen, ppframe);
}

bool is_valid_cmd(char cmd){

    
    bool bvalid = false;	
    int i = 0;

     
    for (i = 0; i < (int)(sizeof(childboard_cmds)/sizeof(childboard_cmds[0])); i++)
    {
	if ((char)childboard_cmds[i] == (char)cmd){
	    bvalid = true;
	    break;
	}
    }

    if (bvalid){
       return true;
    }

    for (i = 0; i < (int)(sizeof(mainboard_cmds)/sizeof(mainboard_cmds[0])); i++)
    {
	if (mainboard_cmds[i] == cmd){

	    bvalid = true;
	    break;
	}
    }
    return bvalid;	
}
