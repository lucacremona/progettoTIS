#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctype.h>

using namespace std;

void searchAndInsertWithPertinence(string *cStr, int size, int weight, MYSQL *conn) {
    string queryStr = "INSERT IGNORE INTO mfw.mfw_nb_similarity(SELECT * , " + std::to_string(weight) + " FROM mfw.mfw_no_bot WHERE mfw_id IN(SELECT DISTINCT(mfw_id) FROM mfw.mfw_no_bot WHERE(contents LIKE '" + cStr[0] + "'";


    for (int i = 1; i < size; i++) {

        queryStr = queryStr + " OR contents LIKE '" + cStr[i] + "'";

    }
    queryStr = queryStr + ")));";
    cout << queryStr << endl;

    if (mysql_query(conn, queryStr.c_str())) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
}

//Main function

int main() {

    string c100[] = {"%Milan%Fashion%Week%", "%#%MFW%", "%Settimana%della%Moda%Milano"};
    string c80[] = {"%#%FW15%", "%Settimana%della%Moda%15"};
    string c60[] = {"%FW%", "%Settimana%della%Moda%", "%Milan%Moda%", "%Camera%Moda", "%Fashion%Week%"};
    string c40[] = {"%Moda%", "%Fashion%", "%AW%15%"};
    string c20[] = {"%Sfilate%", "%Design%", "%Stile%", "%Trend%", "%Collection%", "%collezione%", "%smith%", "%galliano%", "%hilfiger%", "%fiorucci%", "%wang%", "%karan%", "%prada%", "%facchinetti%", "%lagerfeld%", "%armani%", "%balenciaga%", "%tod%s%", "%nannini%", "%fay%", "%miu%", "%moschino%", "%pucci%", "%ford%", "%viktor&rolf%", "%desquared%", "%pinko%", "%roccobarocco%", "%velentino%", "%dolce%", "%gabbana%", "%cavalli%", "%biagiotti%", "%soprani%", "%ferragamo%", "%ferrè%", "%laurent%", "%zegna%", "%westwood%", "%krizia%", "%riva%", "%martini%", "%schon%", "%dior%", "%gaultier%", "%versace%", "%chanel%", "%gucci%", "%prada%", "%desigual%", "%versace%", "%blumarine%", "%moncler%", "%richmond%", "%rinaldi%", "%etro%", "%vuitton%", "%marani%", "%shoe%", "%sisley%", "%twin-set%", "%chloè%", "%barbonese%", "%stefanel%", "%guess%", "%klein%", "%fendi%", "%kenzo%", "%conti%", "%diesel%", "%trussardi%", "%black%", "%byblos%", "%glenfield%", "%timberland%", "%jo%", "%rose%", "%max&co%", "%pepe%", "%exte%", "%orciani%", "%missoni%", "%sempione%", "%barcelona%", "%matiè%", "%laltramoda%", "%merrel%", "%paul&shark%", "%seventy%", "%gattinoni%", "%pirelli%", "%gas%", "%rykiel%", "%derencuny%", "%sails%", "%blauer%", "%woolrich%", "%frani%", "%gigli%", "%sander%", "%mara%", "%malo%", "%cardin%", "%choo%", "%laboutin%", "%burberry%", "%piana%", "%iceberg%", "%barbonese%", "%queen%", "%yachting%", "%marras%", "%repetto%", "%hackerman%", "%lacoste%", "%martina%", "%belstaff%"};

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    char *user = "root";
    char *password = "melone"; /* set me first */
    char *database = "mfw";
    conn = mysql_init(NULL);
    /* Connect to database */
    if (!mysql_real_connect(conn, server,
            user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /*Initialize database*/
    if (mysql_query(conn, "DROP TABLE IF EXISTS mfw.mfw_no_bot;")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "CREATE TABLE mfw.mfw_no_bot LIKE mfw.mfw;")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* Delete BOT User*/
    if (mysql_query(conn, "INSERT INTO mfw.mfw_no_bot (SELECT * from mfw.mfw where mfw_id in( SELECT mfw_id FROM mfw.mfw WHERE mfw_id NOT IN (SELECT mfw_id FROM mfw.mfw WHERE contents LIKE 'RT @%' GROUP BY host HAVING COUNT(*) > 15) GROUP BY host HAVING COUNT(*) < 30))")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /*Initialize database*/
    if (mysql_query(conn, "DROP TABLE IF EXISTS mfw.mfw_nb_similarity;")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    if (mysql_query(conn, "CREATE TABLE mfw.mfw_nb_similarity LIKE mfw.mfw_no_bot;")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /*Add pertinenza field*/
    if (mysql_query(conn, "ALTER TABLE `mfw`.`mfw_nb_similarity` ADD COLUMN `pertinenza` INT(10) NULL DEFAULT 0 AFTER `contents`")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /*Search and set pertinenza field*/
    searchAndInsertWithPertinence(c100, sizeof (c100) / sizeof (*c100), 100, conn);
    searchAndInsertWithPertinence(c80, sizeof (c80) / sizeof (*c80), 80, conn);
    searchAndInsertWithPertinence(c60, sizeof (c60) / sizeof (*c60), 60, conn);
    searchAndInsertWithPertinence(c40, sizeof (c40) / sizeof (*c40), 40, conn);
    searchAndInsertWithPertinence(c20, sizeof (c20) / sizeof (*c20), 20, conn);

    /*query result for histogram */
    if (mysql_query(conn, "SELECT pertinenza, count(*) FROM mfw.mfw_nb_similarity GROUP BY pertinenza;")) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = mysql_use_result(conn);
    ofstream dataAnalisys("data_analisys.txt");
    while ((row = mysql_fetch_row(res)) != NULL) {
        dataAnalisys << row[0] << " " << row[1] << endl;
    }
    dataAnalisys.close();
    system("gnuplot -e \"set style data histograms; set style fill transparent solid 0.5; set grid;set xlabel 'Pertinence (%)'; set ylabel 'Number of Tweeet'; plot 'data_analisys.txt' u 1:2 smooth freq w boxes; pause mouse \"");

    mysql_close(conn);

    return 0;
}

