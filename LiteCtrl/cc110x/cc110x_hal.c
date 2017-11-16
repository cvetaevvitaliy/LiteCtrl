#include "stm32l0xx_hal.h"
#include "cc110x_adapter.h"
#include "sz_libc.h"
#include "cc110x_reg.h"
#include "cc110x_hal.h"
#include "list.h"
#include "delay_work.h"
#include "module_common.h"
#include "device_handle_data.h"
#include "cc110x_hal.h"
static const int recv_max = 32;
static char rxBuffer[32];
static int irq_state  = 0;
static int cc1101_state;
static int send_flg;

static struct cc1101_msg_list_t snd_lit[8];
static char cc1101_buf[8][28];
static struct list_head cc1101_send_list_commit_head;
static struct list_head cc1101_send_list_waite_head;
static struct list_head cc1101_send_list_free_head;
static struct list_head *cc101_send_pos;

struct delay_work cc1101_msg_delay_work;
static int cc1101_work_state = MODULE_STA_STOP;

struct cc110x_t
{
	int rx_sig_strength;
	int tx_sig_strength;
	int scan_cnt;
	int all_pakage;
	int corect_pakage;
	int error_pakage;

};

static struct cc110x_t m_cc1101_pri;

static void cc1101_irq_handle();
static int get_cc110x_state();
static void set_cc110x_state(int st);

int (*cc1101_handle_recv_data_cb)(char *buf,int buf_len)=0;

static int cc1101_handle_recv_data(char *buf,int buf_len)
{
	if(cc1101_handle_recv_data_cb)
		return cc1101_handle_recv_data_cb(buf,buf_len);
	else
		return -1;
}
void set_cc1101_handle_recv_data(int (*cb_)(char *buf,int buf_len))
{
	cc1101_handle_recv_data_cb = cb_;
}

static int get_send_state()
{
	return send_flg;
}
static void set_send_state(int new_state)
{
	send_flg = new_state;
}
static void set_send_pos(struct list_head * new_pos)
{
	cc101_send_pos =  new_pos;
}
static void set_cc1101_msg_work_state(int new_state)
{
	cc1101_work_state = new_state;
}

static int get_cc1101_msg_work_state()
{
	return cc1101_work_state;
}

static struct list_head *get_send_pos()
{
	return cc101_send_pos;
}
static void cc1101_send(char *buf,int len)
{
//	static int cc_snd_cnt = 0;
//    sz_printk("send cnt %d \n",cc_snd_cnt++);
	cc110x_write_strobe(SIDLE);
    cc110x_write_strobe(SFTX);
	//cc110x_write_t(FIFOTHR,0x40);//TX buf = 61
	cc110x_write_t(TX_FIFO,len);
    cc110x_write(TX_FIFO, buf,len);	//写入要发送的数据
    cc110x_write_strobe(STX);		//进入发送模式发送数据
}
static void cc1101_recv_commit()
{
	cc110x_write_strobe(SIDLE);
	cc110x_write_strobe(SFRX);		//清洗接收缓冲区
	//cc110x_write_t(FIFOTHR,0x4e);//RX 60
	cc110x_write_strobe(SRX);
	set_cc110x_state(CC110X_STATE_RECV);
}
int cc1101_msg_start_work()
{
	if(get_cc1101_msg_work_state()==MODULE_STA_WORK)
		return -1;
	set_cc1101_msg_work_state(MODULE_STA_WORK);
	shedule_delay(&cc1101_msg_delay_work,30);
	CC1101_irq_enable();
	cc1101_recv_commit();
}


static void cc1101_msg_reset_list()
{
	struct list_head *pos,*n;
    DIS_ALL_INT();
	if(list_empty(&cc1101_send_list_waite_head)==0)
	{
		list_for_each_safe(pos, n,&cc1101_send_list_waite_head)
	    {
			list_del_init(pos);
			list_add(pos,&cc1101_send_list_free_head);
	    }
		//INIT_LIST_HEAD(&cc1101_send_list_waite_head);
	}
	
	if(list_empty(&cc1101_send_list_commit_head)==0)
	{
		list_for_each_safe(pos, n,&cc1101_send_list_commit_head)
	    {
			list_del_init(pos);
			list_add(pos,&cc1101_send_list_free_head);
	    }
		//INIT_LIST_HEAD(&cc1101_send_list_commit_head);
	}
	EN_ALL_INT();
}

void init_gloabel_info()
{
	m_cc1101_pri.scan_cnt = 0;
	m_cc1101_pri.rx_sig_strength = 0;
	m_cc1101_pri.all_pakage = 0;
	m_cc1101_pri.corect_pakage=0;
	m_cc1101_pri.error_pakage = 0;

}

void cc1101_set_rx_sig_strength(int sig_s)
{
	m_cc1101_pri.rx_sig_strength = sig_s;
}

int cc1101_get_rx_sig_strength()
{
	return m_cc1101_pri.rx_sig_strength;
}
void cc1101_set_tx_sig_strength(int sig_s)
{
	m_cc1101_pri.tx_sig_strength = sig_s;
}

int cc1101_get_tx_sig_strength()
{
	return m_cc1101_pri.tx_sig_strength;
}

void set_pakage_cnt(int flg,int strength)
{
	m_cc1101_pri.all_pakage++;

	if(flg == 0)
	{
		m_cc1101_pri.corect_pakage++;
	}
	else
	{
		m_cc1101_pri.error_pakage++;
	}
	cc1101_set_rx_sig_strength(strength);
}

void set_scan_cnt()
{
	m_cc1101_pri.scan_cnt++;

}


void printf_cc1101_info()
{
	float rate;
	int temp;
		
	sz_printf("rx_sig_strength   :%d\r\n",m_cc1101_pri.rx_sig_strength);
	sz_printf("scan count        :%d\r\n",m_cc1101_pri.scan_cnt);
	sz_printf("recv all pagake   :%d\r\n",m_cc1101_pri.all_pakage);
	sz_printf("correct  pageke   :%d\r\n",m_cc1101_pri.corect_pakage);
	sz_printf("error    pageke   :%d\r\n",m_cc1101_pri.error_pakage);
	if(m_cc1101_pri.all_pakage >0)
	{
		rate = ((float)m_cc1101_pri.error_pakage)/((float)m_cc1101_pri.all_pakage);
		rate*=100;
		temp = (int)rate;
		sz_printf("error    rate  :%d.",temp);
		rate-=(float)temp;
		rate*=10;
		temp = (int)rate;
		sz_printf("%d",temp);
		rate-=(float)temp;
		rate*=10;
		temp = (int)rate;
		sz_printf("%d",temp);
		rate-=(float)temp;
		rate*=10;
		temp = (int)rate;
		sz_printf("%d%\r\n",temp);
	}
	
}
int cc1101_msg_stop_work()
{
	if(get_cc1101_msg_work_state()==MODULE_STA_STOP)
		return -1;
	set_cc1101_msg_work_state(MODULE_STA_STOP);
	cancel_delay_work(&cc1101_msg_delay_work);
	CC1101_irq_disable();
	irq_state = 0;
	cc1101_msg_reset_list();
	return 0;
}



static void cc1101_thread(struct delay_work *p_dwk)
{
	struct list_head *pos,*n,*head;
	struct cc1101_msg_list_t *p_cc110_msg;

	head = &cc1101_send_list_waite_head;
	if(list_empty(head)==0)
	{
		//sz_printk("move waite to work\n");
	    list_for_each_safe(pos, n,head)
	    {
			p_cc110_msg = ContainOf(pos, struct cc1101_msg_list_t, m_list);
    		if(p_cc110_msg->cc1101_msg_ifn.send_ifn.time_interval > 1)
            {
            	p_cc110_msg->cc1101_msg_ifn.send_ifn.time_interval--;
				continue;
            }
			//sz_printk("get one to commit\n");
			list_del_init(pos);
			//__disable_irq();
			list_add(pos,&cc1101_send_list_commit_head);
			__enable_irq();
	    }
	}
	else
	{
		//sz_printk("waite que is empty\n");
	}
	head = &cc1101_send_list_commit_head;
	if(list_empty(head)==0)
	{
    	//__disable_irq();
    	if(get_cc110x_state()==CC110X_STATE_RECV)
    	{
             pos = head->next;
			set_send_pos(pos);
			 p_cc110_msg = ContainOf(pos, struct cc1101_msg_list_t, m_list);
			 p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt--;
			 cc1101_send(p_cc110_msg->buf,p_cc110_msg->len);//is send ok
			 //sz_printk("1.msg num:%d send cnt:%d\n",p_cc110_msg->num,p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt);
			 set_cc110x_state(CC110X_STATE_SEND);
    	}
    	//__enable_irq();
	}
	shedule_delay(&cc1101_msg_delay_work,1);
}


int cc101_msg_send_commit(struct cc1101_msg_list_t *p_cc110_msg,int cnt,int time_intervel)
{
	if(cnt<0)
		cnt = 1;
	if(cnt>5)
		cnt =5;
	p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt = cnt;
	p_cc110_msg->cc1101_msg_ifn.send_ifn.time_interval = time_intervel;
	list_add(&p_cc110_msg->m_list,&cc1101_send_list_waite_head);
	//sz_printk("add one msg to waite que\n");
	return 0;
}

static int cc1101_irq_send_data()
{
	struct list_head *pos,*n,*head;
	struct cc1101_msg_list_t *p_cc110_msg;
	//sz_printk("send data\n");
	pos = get_send_pos();
	if(pos==0 || pos==&cc1101_send_list_commit_head) 
	{
		//sz_printk("1.pos is null\n");
		return -1;
	}
	p_cc110_msg = ContainOf(pos, struct cc1101_msg_list_t, m_list);
    if(p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt > 0)
    {
		cc1101_send(p_cc110_msg->buf,p_cc110_msg->len);
		p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt-=1;
		//sz_printk("2.msg num:%d send cnt:%d\n",p_cc110_msg->num,p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt);
		return 0;
    }
	pos = pos->next;
	set_send_pos(pos);
    cc1101_msg_free_send_msg(p_cc110_msg);//is send ok
    if(pos==&cc1101_send_list_commit_head)
    {
		return -1;
    }
	
	p_cc110_msg = ContainOf(pos, struct cc1101_msg_list_t, m_list);
	cc1101_send(p_cc110_msg->buf,p_cc110_msg->len);
	//sz_printk("3.msg num:%d send cnt:%d\n",p_cc110_msg->num,p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt);
	p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt-=1;
	return 0;
	
}



struct cc1101_msg_list_t *cc1101_msg_alloc_send_msg()
{
	struct cc1101_msg_list_t *p_cc110_msg;
	struct list_head *p_list;
	if(get_cc1101_msg_work_state()==MODULE_STA_STOP)
		return 0;
	if(list_empty(&cc1101_send_list_free_head))
	{
		sz_printk("msg buf is use up\n");
		return 0;
	}
	p_list = cc1101_send_list_free_head.next;
	list_del_init(p_list);
	p_cc110_msg = ContainOf(p_list, struct cc1101_msg_list_t, m_list);
	return p_cc110_msg;
}

void cc1101_msg_free_send_msg(struct cc1101_msg_list_t *p_cc110_msg)
{
	if(p_cc110_msg)
	{
		list_del_init(&p_cc110_msg->m_list);
		list_add_tail(&p_cc110_msg->m_list,&cc1101_send_list_free_head);
	}
}

//struct cc1101_msg_list_t *cc1101_get_send_msg()
//{
//	struct cc1101_msg_list_t *p_cc110_msg;
//	struct list_head *p_list;
//	if(list_empty(&cc1101_send_list_commit_head))
//	{
//		return 0;


//	}
//	p_list = cc1101_send_list_commit_head.next;
//	p_cc110_msg = ContainOf(p_list, struct cc1101_msg_list_t, m_list);
//	if(p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt==0)
//	{
//		cc1101_msg_free_send_msg(p_cc110_msg);
//		if(list_empty(&cc1101_send_list_commit_head))
//    	{
//    		return 0;


//    	}
//    	p_list = cc1101_send_list_commit_head.next;
//    	p_cc110_msg = ContainOf(p_list, struct cc1101_msg_list_t, m_list);
//	}
//	p_cc110_msg->cc1101_msg_ifn.send_ifn.send_cnt -= 1;
//	return p_cc110_msg;
//}

//int cc110x_module_recv_commit()
//{
//	//HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
//	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
//	cc110x_tx_disable();
//    cc110x_rx_enable();
//	cc110x_write_strobe(SIDLE);
//    cc110x_write_strobe(SRX);		//进入接收状态
//    return 0;
//}
//
//int cc110x_module_recv_commit_t()
//{
//	cc110x_tx_disable();
//    cc110x_rx_enable();
//	cc110x_write_strobe(SIDLE);
//    cc110x_write_strobe(SRX);		//进入接收状态
//    return 0;
//}

static int get_cc110x_state()
{
	return cc1101_state;
}
static void set_cc110x_state(int st)
{

	cc1101_state = st;
}

//int cc110x_module_send_commit_t() 
//{
//	//HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
//	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
//	//HAL_Delay(100);
//	rxBuffer[0]=0xFF;
//	cc110x_write_t(ADDR,0x18);
//	print_b(rxBuffer, 8);
//	cc110x_write_strobe(SIDLE);
//	cc110x_write_t(TX_FIFO, 8);
//    cc110x_write(TX_FIFO, rxBuffer, 8);	//写入要发送的数据
//    cc110x_write_strobe(STX);		//进入发送模式发送数据
//}
//int cc110x_module_send_commit() 
//{
//	rxBuffer[0]=0xFF;
//	cc110x_write_t(ADDR,0x18);
//	print_b(rxBuffer, 8);
//	cc110x_write_strobe(SIDLE);
//	cc110x_write_t(TX_FIFO, 8);
//    cc110x_write(TX_FIFO, rxBuffer, 8);	//写入要发送的数据
//    cc110x_write_strobe(STX);		//进入发送模式发送数据
//}


static int cc1101_irq_recv_data(char *buf,int len)
{
	static int cnt=0;
    char status[2];
    int packetLength;
	static int m_cnt = 0;
	
    if ((cc110x_read_status(RXBYTES) & BYTES_IN_RXFIFO)) //如果接的字节数不为0
	{
        packetLength = cc110x_read_t(RX_FIFO);//读出第一个字节，此字节为该帧数据长度
        if (packetLength <= len) 		//如果所要的有效数据长度小于等于接收到的数据包的长度
		{
			//读出所有接收到的数据
            cc110x_read(RX_FIFO, buf, packetLength);         
            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            cc110x_read(RX_FIFO, status, 2); 	//读出CRC校验位
//			sz_printk("status:%#x\n",status[1]);
            if(status[1] & CRC_OK)		//如果校验成功返回接收成功
            {
//				if(++m_cnt%20==0)
//				{
//				    sz_printk("%d.recv data OK,len:%d\n\n",m_cnt++,packetLength);
//				    print_b(buf, packetLength);
//				}
				set_pakage_cnt(0,status[0]);
				return packetLength;
            }
			else
			{
				set_pakage_cnt(-1,status[0]);
//				sz_printk("CRC err.\n");
//                print_b(rxBuffer, packetLength);
//                sz_printk("\n");
			}
        }
		 else 
		{
			//sz_printk("1.there no data,cnt:%d\n",cnt);
            //cc110x_write_strobe(SFRX);		//清洗接收缓冲区
        }
    } 
	else
	{
		//sz_printk("2.there no data,cnt:%d\n",cnt);
	}
	return -1;
}

static void cc1101_irq_handle()
{
    int ret;
	if(irq_state==1)
		irq_state = 0;
	else
		return ;
//	//sz_printk("2.-----\n");
    switch(get_cc110x_state())
    {
        case CC110X_STATE_RECV:
			ret = cc1101_irq_recv_data(rxBuffer,recv_max);
			if(ret!=-1)
			{
				//handle data
				cc1101_handle_recv_data(rxBuffer,ret);
				//cc1101_handle_data(rxBuffer,ret);
				//sz_memset(rxBuffer, 0,recv_max);
			}
			cc110x_write_strobe(SIDLE);
			cc110x_write_strobe(SFRX);		//清洗接收缓冲区
			//cc110x_write_t(FIFOTHR,0x4e);   //TX buf = 61
			cc110x_write_strobe(SRX);
			break;
		case CC110X_STATE_SEND:
			if(cc1101_irq_send_data()==-1)
			{
                set_cc110x_state(CC110X_STATE_RECV);
				irq_state = 0;
				//systick_delay(100);
				cc110x_write_strobe(SIDLE);
				//cc110x_write_t(FIFOTHR,0x4e);//TX buf = 61
			    cc110x_write_strobe(SFRX);		//清洗接收缓冲区
			    cc110x_write_strobe(SRX);	
			}
			break;
		default:
			sz_printk("cc1101 state:%d\n",get_cc110x_state());
			break;		

    }
}


//void EXTI1_IRQHandler(void)
//{
//	cc1101_irq_handle();
//    CC1101_CLEAR_IRQ(); 
//}


void EXTI2_3_IRQHandler(void)
{
	//cc1101_irq_handle();
	irq_state = 1;
	//sz_printk("1.-----\n");
   // EXTI_ClearITPendingBit(EXTI_Line8);
   HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3); 
}


int cc1101_msg_init()
{
	int i;
	INIT_LIST_HEAD(&cc1101_send_list_commit_head);
	INIT_LIST_HEAD(&cc1101_send_list_waite_head);
    INIT_LIST_HEAD(&cc1101_send_list_free_head);
	for(i=0;i<8;i++)
	{
		snd_lit[i].max = 28;
		snd_lit[i].buf = cc1101_buf[i];
		snd_lit[i].num = i;
		list_add(&snd_lit[i].m_list,&cc1101_send_list_free_head);
	}
	set_send_state(CC1101_STATE_SEND_IDLE);
	set_send_pos(0);
	set_cc1101_msg_work_state(MODULE_STA_STOP);
    shedule_init(&cc1101_msg_delay_work);
	cc1101_msg_delay_work.handle = cc1101_thread;
	return 0;
}

void cc1101_msg_entery()
{
	cc1101_irq_handle();
}

void msg_printf()
{
	struct list_head *pos,*n,*head;
	
	int i=0;
	
	head = &cc1101_send_list_commit_head;
	i=0;
	if(list_empty(head)==0)
	{
	    list_for_each_prev_safe(pos,n,head)
	    {
			i++;
	    }
	}
	sz_printk("msg send list cnt:%d\n",i);

    head = &cc1101_send_list_waite_head;
	i=0;
	if(list_empty(head)==0)
	{
		i=0;
	    list_for_each_prev_safe(pos,n,head)
	    {
			i++;
	    }
	}
	sz_printk("msg waite list cnt:%d\n",i);


    head = &cc1101_send_list_free_head;
	i=0;
	if(list_empty(head)==0)
	{
		i=0;
	    list_for_each_prev_safe(pos,n,head)
	    {
			i++;
	    }
		sz_printk("msg free list cnt:%d\n",i);
	}
	
}

//==================================================================================
// 功能描述 : RSSI值计算
// 输入参数 : rssidata:RSSI寄存器读取值              rssioffset 取值75-79
// 返回参数 : dBm值  值越大信号越差
// 说明     : 将寄存器中的二进制值装换成dBm值
//==================================================================================
const int rssioffset = 78;
int RSSI_calculated(int rssidata)
{   
	int  rssirusel;   
	if(rssidata >= 128)
	{
		rssirusel=((rssidata-256)/2)-rssioffset; 
	}
	else 
	{
		rssirusel =((int)rssidata/ 2)-rssioffset; 
	}
	return  -rssirusel;//装换成正数 
}
