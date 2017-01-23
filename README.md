## Area Of Interest


打格子： 不灵活，大地图耗内存。 但是计算高效

十字链： 每次移动都要更新两个双向链表， 耗CPU


所以结合打格子和十字链的优点，得到这个 **栅栏算法**.

#### 栅栏算法

只在一个方向分割地图 （栅栏）, 分割后，每个区域用 skip list 管理



benchmark:
```
model       : 42
model name  : Intel(R) Core(TM) i5-2430M CPU @ 2.40GHz
model       : 42
model name  : Intel(R) Core(TM) i5-2430M CPU @ 2.40GHz
model       : 42
model name  : Intel(R) Core(TM) i5-2430M CPU @ 2.40GHz
model       : 42
model name  : Intel(R) Core(TM) i5-2430M CPU @ 2.40GHz
```

2w x 2w 的地图， 2w个实体，100的aoi范围， 每个实体依次随机移动并获取自己的aoi列表
```
#1
move cost: 20909 us
aoi cost: 45164 us
total cost: 66.072998 ms

#2
move cost: 21876 us
aoi cost: 35892 us
total cost: 57.768002 ms

#2
move cost: 18042 us
aoi cost: 31727 us
total cost: 49.769001 ms

#2
move cost: 17890 us
aoi cost: 28654 us
total cost: 46.543999 ms

#2
move cost: 16235 us
aoi cost: 29742 us
total cost: 45.977001 ms
```

