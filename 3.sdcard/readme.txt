1.用仿真器烧运行时，串口会打印:led start
2.同时两个LED交替闪烁
3.将程序中，设置MMDC相关的部分删除了
4.借助u-boot可启动并且初始化DDR
5.可以用mkimage.sh打包成tf卡的img文件，烧写进去。