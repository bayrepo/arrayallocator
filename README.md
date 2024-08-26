Больше информации: [inarrayalloc](https://brepo.ru/post/inarray-allocator-opisanie)

Описание
=====================

Библиотека в составе которой расположены исходники альтернативного аллокатора памяти в пределах ранее созданного массива.


Сборка
=====================

Для сборки необходимо установленные пакеты CUnit, CUnit-devel, cmake, gcc. Приведу на примере сборки CentOS 7


Последовательность сборки
=====================

- yum install CUnit CUnit-devel cmake gcc git
- git clone https://github.com/bayrepo/arrayallocator.git
- cd arrayallocator/build
- cmake ..
- make
- make unit-test
- make install


Установка из пакета
=====================

Последовательность установки CentOS 9/Rhel 9 based systems:

```
echo -e "[repomanager-bayrepo_prod]
name=bayrepo repo on repo.brepo.ru
baseurl=https://repo.brepo.ru/repo/bayrepo_prod
enabled=1
gpgkey=https://repo.brepo.ru/repo/gpgkeys/repo.brepo.ru.pub
gpgcheck=1" > /etc/yum.repos.d/repomanager-bayrepo.repo
```
- yum install inarray-allocator inarray-allocator-devel -y
- установка завершена


Примеры использования
=====================

Примеры использования располжены в папке examples

* hash_in_shmem.c - размещение hash-таблицы в shared memory с доступом к ней разных процессов
* list_in_array.c - пример сохранения связного списка в стеке(в ограниченном массиве созданном на стеке), без оспользования alloca

В качестве алгоритмов реализации hash, linked-list, ringbuffer, динамеческого массива и динамических строк использована доработанная библиотека https://troydhanson.github.io/uthash/

