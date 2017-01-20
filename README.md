## Area Of Interest

#### aoi.cpp

打格子： 不灵活，大地图耗内存。 但是计算高效

十字链： 每次移动都要更新两个双向链表， 耗CPU


所以结合打格子和十字链的优点，得到这个 **栅栏算法**.


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

10000 的地图边界， 3000个实体，80的aoi范围， 每个实体依次随机移动并获取aoi列表
```
#1
move cost: 1244 us
aoi cost: 5185 us
total cost: 6.429000 ms

#2
move cost: 1228 us
aoi cost: 3979 us
total cost: 5.207000 ms

#3
move cost: 1822 us
aoi cost: 5745 us
total cost: 7.567000 ms

#4
move cost: 3196 us
aoi cost: 6600 us
total cost: 9.796000 ms

#5
move cost: 1285 us
aoi cost: 4863 us
total cost: 6.148000 ms
```

100000 的地图边界， 10000个实体，200的aoi范围， 每个实体依次随机移动并获取aoi列表
```
#1
move cost: 5139 us
aoi cost: 39043 us
total cost: 44.181999 ms

#2
move cost: 6461 us
aoi cost: 24101 us
total cost: 30.562000 ms

#3
move cost: 6013 us
aoi cost: 38996 us
total cost: 45.008999 ms

#4
move cost: 5446 us
aoi cost: 39850 us
total cost: 45.296001 ms

#5
move cost: 4153 us
aoi cost: 16326 us
total cost: 20.479000 ms
```

