#include <stdio.h>
#include <string.h>
#include "graph.h"

#define STR_BUFF_MAX 64

const char beijing[] = "北京";
const char shijiazhuang[] = "石家庄";
const char taiyuan[] = "太原";
const char zhengzhou[] = "郑州";
const char shanghai[] = "上海";
const char shenyang[] = "沈阳";
const char tianjin[] = "天津";
const char hushi[] = "呼和浩特";
const char nanjing[] = "南京";
const char wuhan[] = "武汉";
const char changsha[] = "长沙";
const char shenzhen[] = "深圳";
const char guangzhou[] = "广州";
const char nanchang[] = "南昌";
const char xian[] = "西安";
const char chongqing[] = "重庆";
const char chengdu[] = "成都";
const char kunming[] = "昆明";
const char lasa[] = "拉萨";
const char wulumuqi[] = "乌鲁木齐";
const char changchun[] = "长春";
const char haerbin[] = "哈尔滨";
const char jinan[] = "济南";
const char hefei[] = "合肥";
const char hangzhou[] = "杭州";
const char yinchuan[] = "银川";
const char xining[] = "西宁";
const char lanzhou[] = "兰州";
const char guiyang[] = "贵阳";
const char nanning[] = "南宁";
const char haikou[] = "海口";
const char fuzhou[] = "福州";
const char xianggang[] = "香港";
const char aomen[] = "澳门";
const char taibei[] = "台北";

const char *city_list[] = {
    beijing,
    shijiazhuang,
    taiyuan,
    zhengzhou,
    shanghai,
    shenyang,
    tianjin,
    hushi,
    nanjing,
    wuhan,
    changsha,
    shenzhen,
    guangzhou,
    nanchang,
    xian,
    chongqing,
    chengdu,
    kunming,
    lasa,
    wulumuqi,
    changchun,
    haerbin,
    jinan,
    hefei,
    hangzhou,
    yinchuan,
    xining,
    lanzhou,
    guiyang,
    nanning,
    haikou,
    fuzhou,
    xianggang,
    aomen,
    taibei,
    NULL
};

/* 构建地理信息 */
void build_geography_data(GRAPH *graph);

/* 选择输入整数 */
int input_int(const char *hint, int def);

/* 显示城市列表 */
void show_city_list();

const char *print_address(void *data)
{
    return (const char *)data;
}

/* 广度优先搜索 */
void test_bfs()
{
    GRAPH *graph = graph_create(0);

    /* 构建城市信息 */
    build_geography_data(graph);

    while (1) {
        int src = 0;
        int dest = 0;

        show_city_list();

        src = input_int("请选择你的出发地？", 0);
        dest = input_int("请选择你的目的地？", 0);

        /* 广度优先遍历后再打印路径 */
        graph_bfs_print(graph, src, dest, print_address);

        if (!input_int("是否继续？ 0.退出 1.继续", 1)) {
            break;
        }
    }

    graph_clear_adjacent(graph);
    graph_destroy(graph);
}

/* 构建地理信息 */
void build_geography_data(GRAPH *graph)
{
    graph_push_data(graph, (void *)beijing);        /* 0 */
    graph_push_data(graph, (void *)shijiazhuang);   /* 1 */
    graph_push_data(graph, (void *)taiyuan);        /* 2 */
    graph_push_data(graph, (void *)zhengzhou);      /* 3 */
    graph_push_data(graph, (void *)shanghai);       /* 4 */
    graph_push_data(graph, (void *)shenyang);       /* 5 */
    graph_push_data(graph, (void *)tianjin);        /* 6 */
    graph_push_data(graph, (void *)hushi);          /* 7 */
    graph_push_data(graph, (void *)nanjing);        /* 8 */
    graph_push_data(graph, (void *)wuhan);          /* 9 */
    graph_push_data(graph, (void *)changsha);       /* 10 */
    graph_push_data(graph, (void *)shenzhen);       /* 11 */
    graph_push_data(graph, (void *)guangzhou);      /* 12 */
    graph_push_data(graph, (void *)nanchang);       /* 13 */
    graph_push_data(graph, (void *)xian);           /* 14 */
    graph_push_data(graph, (void *)chongqing);      /* 15 */
    graph_push_data(graph, (void *)chengdu);        /* 16 */
    graph_push_data(graph, (void *)kunming);        /* 17 */
    graph_push_data(graph, (void *)lasa);           /* 18 */
    graph_push_data(graph, (void *)wulumuqi);       /* 19 */
    graph_push_data(graph, (void *)changchun);      /* 20 */
    graph_push_data(graph, (void *)haerbin);        /* 21 */
    graph_push_data(graph, (void *)jinan);          /* 22 */
    graph_push_data(graph, (void *)hefei);          /* 23 */
    graph_push_data(graph, (void *)hangzhou);       /* 24 */
    graph_push_data(graph, (void *)yinchuan);       /* 25 */
    graph_push_data(graph, (void *)xining);         /* 26 */
    graph_push_data(graph, (void *)lanzhou);        /* 27 */
    graph_push_data(graph, (void *)guiyang);        /* 28 */
    graph_push_data(graph, (void *)nanning);        /* 29 */
    graph_push_data(graph, (void *)haikou);         /* 30 */
    graph_push_data(graph, (void *)fuzhou);         /* 31 */
    graph_push_data(graph, (void *)xianggang);      /* 32 */
    graph_push_data(graph, (void *)aomen);          /* 33 */
    graph_push_data(graph, (void *)taibei);         /* 34 */

    /* 北京 */
    graph_set_adjacent(graph, 0, 1);
    graph_set_adjacent(graph, 0, 7);
    graph_set_adjacent(graph, 0, 5);
    graph_set_adjacent(graph, 0, 6);

    /* 石家庄 */
    graph_set_adjacent(graph, 1, 0);
    graph_set_adjacent(graph, 1, 2);
    graph_set_adjacent(graph, 1, 3);
    graph_set_adjacent(graph, 1, 22);
    graph_set_adjacent(graph, 1, 6);

    /* 太原 */
    graph_set_adjacent(graph, 2, 1);
    graph_set_adjacent(graph, 2, 3);
    graph_set_adjacent(graph, 2, 7);
    graph_set_adjacent(graph, 2, 14);
    graph_set_adjacent(graph, 2, 25);

    /* 郑州 */
    graph_set_adjacent(graph, 3, 1);
    graph_set_adjacent(graph, 3, 2);
    graph_set_adjacent(graph, 3, 22);
    graph_set_adjacent(graph, 3, 14);
    graph_set_adjacent(graph, 3, 23);
    graph_set_adjacent(graph, 3, 8);
    graph_set_adjacent(graph, 3, 9);

    /* 上海 */
    graph_set_adjacent(graph, 4, 8);
    graph_set_adjacent(graph, 4, 24);
    graph_set_adjacent(graph, 4, 22);

    /* 沈阳 */
    graph_set_adjacent(graph, 5, 0);
    graph_set_adjacent(graph, 5, 6);
    graph_set_adjacent(graph, 5, 20);

    /* 天津 */
    graph_set_adjacent(graph, 6, 0);
    graph_set_adjacent(graph, 6, 5);
    graph_set_adjacent(graph, 6, 1);
    graph_set_adjacent(graph, 6, 22);

    /* 呼市 */
    graph_set_adjacent(graph, 7, 0);
    graph_set_adjacent(graph, 7, 1);
    graph_set_adjacent(graph, 7, 2);
    graph_set_adjacent(graph, 7, 25);

    /* 南京 */
    graph_set_adjacent(graph, 8, 22);
    graph_set_adjacent(graph, 8, 23);
    graph_set_adjacent(graph, 8, 3);
    graph_set_adjacent(graph, 8, 4);
    graph_set_adjacent(graph, 8, 24);

    /* 武汉 */
    graph_set_adjacent(graph, 9, 3);
    graph_set_adjacent(graph, 9, 14);
    graph_set_adjacent(graph, 9, 23);
    graph_set_adjacent(graph, 9, 13);
    graph_set_adjacent(graph, 9, 10);
    graph_set_adjacent(graph, 9, 15);

    /* 长沙 */
    graph_set_adjacent(graph, 10, 9);
    graph_set_adjacent(graph, 10, 13);
    graph_set_adjacent(graph, 10, 15);
    graph_set_adjacent(graph, 10, 28);
    graph_set_adjacent(graph, 10, 12);
    graph_set_adjacent(graph, 10, 29);
    graph_set_adjacent(graph, 10, 31);

    /* 深圳 */
    graph_set_adjacent(graph, 11, 12);
    graph_set_adjacent(graph, 11, 32);
    graph_set_adjacent(graph, 11, 33);

    /* 广州 */
    graph_set_adjacent(graph, 12, 11);
    graph_set_adjacent(graph, 12, 33);
    graph_set_adjacent(graph, 12, 30);
    graph_set_adjacent(graph, 12, 29);
    graph_set_adjacent(graph, 12, 10);
    graph_set_adjacent(graph, 12, 13);
    graph_set_adjacent(graph, 12, 31);
    graph_set_adjacent(graph, 12, 28);

    /* 南昌 */
    graph_set_adjacent(graph, 13, 12);
    graph_set_adjacent(graph, 13, 9);
    graph_set_adjacent(graph, 13, 10);
    graph_set_adjacent(graph, 13, 31);
    graph_set_adjacent(graph, 13, 24);
    
    /* 西安 */
    graph_set_adjacent(graph, 14, 2);
    graph_set_adjacent(graph, 14, 3);
    graph_set_adjacent(graph, 14, 25);
    graph_set_adjacent(graph, 14, 15);
    graph_set_adjacent(graph, 14, 16);
    graph_set_adjacent(graph, 14, 27);
    graph_set_adjacent(graph, 14, 9);

    /* 重庆 */
    graph_set_adjacent(graph, 15, 14);
    graph_set_adjacent(graph, 15, 16);
    graph_set_adjacent(graph, 15, 28);
    graph_set_adjacent(graph, 15, 17);
    graph_set_adjacent(graph, 15, 10);
    graph_set_adjacent(graph, 15, 9);

    /* 成都 */
    graph_set_adjacent(graph, 16, 15);
    graph_set_adjacent(graph, 16, 14);
    graph_set_adjacent(graph, 16, 27);
    graph_set_adjacent(graph, 16, 17);
    graph_set_adjacent(graph, 16, 28);
    graph_set_adjacent(graph, 16, 18);

    /* 昆明 */
    graph_set_adjacent(graph, 17, 16);
    graph_set_adjacent(graph, 17, 28);
    graph_set_adjacent(graph, 17, 29);
    graph_set_adjacent(graph, 17, 18);

    /* 拉萨 */
    graph_set_adjacent(graph, 18, 17);
    graph_set_adjacent(graph, 18, 16);

    /* 乌鲁木齐 */
    graph_set_adjacent(graph, 19, 26);
    graph_set_adjacent(graph, 19, 27);

    /* 长春 */
    graph_set_adjacent(graph, 20, 5);
    graph_set_adjacent(graph, 20, 21);

    /* 哈尔滨 */
    graph_set_adjacent(graph, 21, 20);
    
    /* 济南 */
    graph_set_adjacent(graph, 22, 6);
    graph_set_adjacent(graph, 22, 1);
    graph_set_adjacent(graph, 22, 3);
    graph_set_adjacent(graph, 22, 23);
    graph_set_adjacent(graph, 22, 8);
    graph_set_adjacent(graph, 22, 4);

    /* 合肥 */
    graph_set_adjacent(graph, 23, 22);
    graph_set_adjacent(graph, 23, 3);
    graph_set_adjacent(graph, 23, 8);
    graph_set_adjacent(graph, 23, 9);
    graph_set_adjacent(graph, 23, 13);
    graph_set_adjacent(graph, 23, 24);

    /* 杭州 */
    graph_set_adjacent(graph, 24, 23);
    graph_set_adjacent(graph, 24, 8);
    graph_set_adjacent(graph, 24, 4);
    graph_set_adjacent(graph, 24, 13);
    graph_set_adjacent(graph, 24, 31);

    /* 银川 */
    graph_set_adjacent(graph, 25, 7);
    graph_set_adjacent(graph, 25, 14);
    graph_set_adjacent(graph, 25, 27);
    graph_set_adjacent(graph, 25, 2);

    /* 西宁 */
    graph_set_adjacent(graph, 26, 27);
    graph_set_adjacent(graph, 26, 19);

    /* 兰州 */
    graph_set_adjacent(graph, 27, 26);
    graph_set_adjacent(graph, 27, 19);
    graph_set_adjacent(graph, 27, 25);
    graph_set_adjacent(graph, 27, 14);
    graph_set_adjacent(graph, 27, 15);
    graph_set_adjacent(graph, 27, 16);

    /* 贵阳 */
    graph_set_adjacent(graph, 28, 15);
    graph_set_adjacent(graph, 28, 16);
    graph_set_adjacent(graph, 28, 17);
    graph_set_adjacent(graph, 28, 29);
    graph_set_adjacent(graph, 28, 10);
    graph_set_adjacent(graph, 28, 12);

    /* 南宁 */
    graph_set_adjacent(graph, 29, 28);
    graph_set_adjacent(graph, 29, 17);
    graph_set_adjacent(graph, 29, 10);
    graph_set_adjacent(graph, 29, 12);
    graph_set_adjacent(graph, 29, 30);
    graph_set_adjacent(graph, 29, 33);

    /* 海口 */
    graph_set_adjacent(graph, 30, 29);
    graph_set_adjacent(graph, 30, 12);
    graph_set_adjacent(graph, 30, 33);

    /* 福州 */
    graph_set_adjacent(graph, 31, 24);
    graph_set_adjacent(graph, 31, 13);
    graph_set_adjacent(graph, 31, 10);
    graph_set_adjacent(graph, 31, 12);
    graph_set_adjacent(graph, 31, 34);

    /* 香港 */
    graph_set_adjacent(graph, 32, 11);
    graph_set_adjacent(graph, 32, 33);

    /* 澳门 */
    graph_set_adjacent(graph, 33, 32);
    graph_set_adjacent(graph, 33, 11);
    graph_set_adjacent(graph, 33, 12);
    graph_set_adjacent(graph, 33, 29);

    /* 台北 */
    graph_set_adjacent(graph, 34, 31);
}

int input_int(const char *hint, int def)
{
	static char buff[STR_BUFF_MAX] = { 0 };
	char		*ch = NULL;
	int			ret = -1;
	int			fields = 0;

	if (hint && hint[0])
	{
		printf("%s\n", hint);
	}

	if (!fgets(buff, STR_BUFF_MAX, stdin))
	{
		return -1;
	}

	ch = strrchr(buff, '\n');
	
	if (*ch)
	{
		*ch = 0;
	}

	if (!strlen(buff))
	{
		return def;
	}

	ret = -1;

	if (buff[0] == '0' && buff[1] == 'x')
	{
		ch = buff + 2;
		fields = sscanf(buff, "%X", &ret);
	}
	else
	{
		fields = sscanf(buff, "%d", &ret);
	}

	return (fields > 0) ? ret : -1;
}

/* 显示城市列表 */
void show_city_list()
{
    int i = 0;
    const char *pt = city_list[i];

    while (pt) {
        printf("%d) %s\n", i, pt);
        pt = city_list[++i];
    }
}
