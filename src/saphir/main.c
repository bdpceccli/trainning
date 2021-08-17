#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <mysql/mysql.h>

#define MQTT_PORT 1883
#define MQTT_KEEPALIVE 60

//struct mosquitto *mosq = NULL;
//char *topic = NULL;

/**
 * Initialisation sql
 */
void init_mysql(MYSQL **con, char * host) {

	fprintf(stderr, "MySQL client version: %s\n", mysql_get_client_info());
	
	if ((*con = mysql_init(NULL)) == NULL) {
		fprintf(stderr, "%s\n", mysql_error(*con));
		exit (EXIT_FAILURE);
	}

	if (mysql_real_connect(*con, host, "testuser", "testpass", NULL, 0, NULL, 0) == NULL) {
		fprintf(stderr, "%s\n", mysql_error(*con));
		mysql_close(*con);
		exit(EXIT_FAILURE);
	}
}



/**
 * Initialisation mqtt
 */
void init_mqtt(struct mosquitto **mosq, char * host) {
	bool clean_session = true;
	int status;
	
	// Initialisation de la librairie
	mosquitto_lib_init();
	
	// Création de la structure
	*mosq = mosquitto_new(NULL, clean_session, NULL);
	if (!*mosq) {
		fprintf(stderr, "erreur de creation de la structure mosquitto\n");
		exit (EXIT_FAILURE);
	}
	
	// Association du callback
	//mosquitto_log_callback_set(*mosq, mosq_log_callback);
	
	// Connexion au broker
	status = mosquitto_connect(*mosq, host, MQTT_PORT, MQTT_KEEPALIVE);
	if (status != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "connection au broker %s impossible\n", host);
		if (status == MOSQ_ERR_INVAL)
			fprintf(stderr, "Parametres incorrects\n");
		else
			fprintf(stderr, "%s\n", strerror (errno));
		exit (EXIT_FAILURE);
	}
	
	// Lancement du thread pour reception des subscriptions
	status = mosquitto_loop_start(*mosq);
	if(status != MOSQ_ERR_SUCCESS){
		fprintf(stderr, "lancement du thread impossible\n");
		if (status == MOSQ_ERR_INVAL)
			fprintf(stderr, "Parametres incorrects\n");
		else
			fprintf(stderr, "Pas de support multithread\n");
		exit (EXIT_FAILURE);
	}

//	printf("mosq=0x%x\n", mosq);
}




/**
 * Recupere les parametres
 */
void decode_commandline (int argc, char *argv[], char *host, char *sql) 
{
	int opt;

	if (argc < 5) {
		fprintf(stderr, "Usage %s -h mqtthost -d sqlhost\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	while ((opt = getopt(argc, argv, "h:d:")) != -1) {
		switch (opt) {
			case 'h':
				strcpy(host, optarg);
				break;
			case 'd':
				strcpy(sql, optarg);
				break;
			default:
				fprintf(stderr, "Usage %s -h mqtthost -d sqlhost\n", argv[0]);
				exit (EXIT_FAILURE);
		}
	}
}


int main (int argc, char *argv[])
{
	char broker[256];
	char sql[256];
	struct mosquitto *m = NULL;
	MYSQL *con = NULL;

	decode_commandline(argc, argv, broker, sql);

	// initialisation de la librairie
	//init_mqtt(&m, broker);

	init_mysql(&con, sql);
	mysql_close(con);
	exit(EXIT_SUCCESS);
}
