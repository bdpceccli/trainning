#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#define MQTT_PORT 1883
#define MQTT_KEEPALIVE 60


struct mosquitto *mosq = NULL;
char *topic = NULL;



/**
 *  Fonction de log des échanges mqtt
 */
void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
	// Affichage des transactions mqtt
	switch(level){
		//case MOSQ_LOG_DEBUG:
		//case MOSQ_LOG_INFO:
		//case MOSQ_LOG_NOTICE:
		case MOSQ_LOG_WARNING:
		case MOSQ_LOG_ERR: {
			fprintf(stderr, "%i:%s\n", level, str);
    }
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
	mosquitto_log_callback_set(*mosq, mosq_log_callback);
	
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
 * Envoi d'un message sur le broker
 */
int mqtt_send(struct mosquitto *m, char *topic, char *msg){
	int status;
	
	status = mosquitto_publish(m, NULL, topic, strlen(msg), msg, 0, 0);
	
	if (status != MOSQ_ERR_SUCCESS) {
		fprintf (stderr, "Erreur d'envoi de message mqtt (topic=%s, msg=%s\n)",
			topic, msg);
		fprintf (stderr, "%s\n", mosquitto_strerror(status));
			
		/*switch (status) {
			case MOSQ_ERR_INVAL:
				fprintf(stderr, "input parameters were invalid\n");
				break;
			case MOSQ_ERR_NOMEM:
				fprintf(stderr, "out of memory condition occurred\n");
				break;
			case MOSQ_ERR_NO_CONN:
				fprintf(stderr, "the client isn’t connected to a broker\n");
				break;
			case MOSQ_ERR_PROTOCOL:
				fprintf(stderr, "protocol error communicating with the broker\n");
				break;
			case MOSQ_ERR_PAYLOAD_SIZE:
				fprintf(stderr, "payloadlen is too large\n");
				break;
			case MOSQ_ERR_MALFORMED_UTF8:
				fprintf(stderr, "the topic is not valid UTF-8\n");
				break;
			case MOSQ_ERR_NOT_SUPPORTED:
				fprintf(stderr, "QoS is greater than that supported by the broker\n");
				break;
			case MOSQ_ERR_KEEPALIVE:
				fprintf(stderr, "packet would be larger than supported by the broker\n");
				break;
		}*/
		
	}
	return status;
}

/**
 * Reccupere les parametres
 */
void decode_commandline(int argc, char *argv[], int *id, char *host) {
	int opt;
	
	if (argc < 5) {
		fprintf(stderr, "Usage %s -i busid -h mqtthost\n", argv[0]);
        exit(EXIT_FAILURE);
	}
	
	while ((opt = getopt(argc, argv, "i:h:")) != -1) {
		switch (opt) {
			case 'i':
				*id = atoi(optarg);
				break;
			case 'h':
				strcpy(host, optarg);
				break;
			default:
				fprintf(stderr, "Usage %s -i busid -h mqtthost\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
}


/**
 * main
 */
int main (int argc, char *argv[]) {
	int busid = 0;
	char broker[256];
	struct mosquitto *m = NULL;
	char long_topic[256], latt_topic[256], msg[256];
	double longitude, lattitude;
	
	// reccuperation des parametres
	decode_commandline(argc, argv, &busid, broker);

	// initialisation de la librairie
	init_mqtt(&m, broker);
	
	// initialisation des topics
	sprintf(long_topic, "bus%i/longitude", busid);
	sprintf(latt_topic, "bus%i/lattitude", busid);

	// coordonnées par défaut
	longitude = 43.6932939;
	lattitude = 7.25448874;

	// boucle principale
	while (1) {
		sprintf (msg, "%f", longitude);
		mqtt_send (m, long_topic, msg);
		
		sprintf (msg, "%f", lattitude);
		mqtt_send (m, latt_topic, msg);
		
		longitude += 0.00001;
		lattitude += 0.00001;
		
		usleep(1000000);
	}

}
