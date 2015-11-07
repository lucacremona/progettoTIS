Requirements:
-mysql server (https://www.mysql.it/) or mariadb (https://mariadb.org/) with provided mfw db imported.
-mysql connector for C language (https://dev.mysql.com/downloads/connector/c/)
-gnuplot installed (http://www.gnuplot.info/)

Before starting:
-set correct db account parameter editing main.cpp file

Compile command:
g++ -std=c++11 `PATH_OF_YOUR_MYSQL_CONNECTOR/bin/mysql_config --cflags --include --libs` main.cpp -o result

Run command:
./result

Enjoy!


