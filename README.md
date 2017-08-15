Больше информации: http://bayrepo.net/doku.php?id=ru:inarrayalloc

Описание
=====================

Библиотека в составе которой расположены исходники альтернативного аллокатора памяти в пределах ранее созданного массива.


Сборка
=====================

Для сборки необходимо установленные пакеты CUnit, CUnit-devel, cmake, gcc. Приведу на примере сборки CentOS 7


Последовательность сборки
=====================

yum install CUnit CUnit-devel cmake gcc git
git clone https://github.com/bayrepo/arrayallocator.git
cd arrayallocator/build
cmake ..
make
make unit-test
make install


Установка из пакета
=====================

Последовательность установки CentOS 7
wget http://download.opensuse.org/repositories/home:/bayrepo/CentOS_7/home:bayrepo.repo -O /etc/yum.repos.d/home:bayrepo.repo
yum install inarray-allocator inarray-allocator-devel -y
установка завершена

Последовательность установки CentOS 6
wget http://download.opensuse.org/repositories/home:/bayrepo/CentOS_6/home:bayrepo.repo -O /etc/yum.repos.d/home:bayrepo.repo
yum install inarray-allocator inarray-allocator-devel -y
установка завершена

Примеры использования
=====================

Примеры использования располжены в папке examples

* hash_in_shmem.c - размещение hash-таблицы в shared memory с доступом к ней разных процессов
* list_in_array.c - пример сохранения связного списка в стеке(в ограниченном массиве созданном на стеке), без оспользования alloca

В качестве алгоритмов реализации hash, linked-list, ringbuffer, динамеческого массива и динамических строк использована доработанная библиотека https://troydhanson.github.io/uthash/

