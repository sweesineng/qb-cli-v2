#include <curl/curl.h>
#include <json-c/json.h>
#include "global.h"

/* holder for curl fetch */
struct curl_fetch_st {
	char *Payload;
	size_t Psize;
};
struct curl_fetch_st curl_fetch;

int json_object_count(struct json_object *jobj);
char *URLString(char* u, char* s);
CURLcode curl_fetch_url(CURL *ch, const char *_URL, const char *ref, struct curl_fetch_st *fetch);
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp);
struct json_object * findobj(struct json_object *jobj, const char *key);
void parse_json(char *res);

/* insert path into url */
char *URLString(char* u, char* s) {
	char *buff = malloc((int)(strlen(u) + (int)strlen(s) + 1) * sizeof(char));
	strcpy(buff,u);
	strcat(buff,s);
	return buff;
}

/* Curl and return json data */
int GetData(char EState[100]){
  CURL *CHandle;
  CURLcode CCode;
  struct curl_fetch_st *CFetch = &curl_fetch;
	struct curl_slist *headers = NULL;

	if ((CHandle = curl_easy_init()) == NULL) {
		strcpy(EState,"Error 1: Failed to create curl handle in fetch_session");
		return 1;
	}

	/* Set ref = URL if is empty */
	if(ref == NULL) {
		ref = _URL;
	}

  char *URL_String = URLString(_URL, "/sync/maindata");
  CCode = curl_fetch_url(CHandle, URL_String, ref, CFetch);	/* fetch page and capture return code */

	curl_easy_cleanup(CHandle); /* cleanup curl handle */
	curl_slist_free_all(headers);

	if (CCode != CURLE_OK || CFetch->Psize < 1) {
		sprintf(EState,"Error 2: Failed to fetch %s", URL_String);
		free(URL_String);
		return 2;
	}

  if (CFetch->Payload != NULL) {
	  free(URL_String);
	  parse_json(CFetch->Payload);
	  free(CFetch->Payload);	/* free payload */
	  return 0;
  }else{
	  free(URL_String);
	  strcpy(EState,"Failed to populate payload");
	  return 3;
  }
}

/* fetch and return url body via curl */
CURLcode curl_fetch_url(CURL *CHandle, const char *URL, const char *ref, struct curl_fetch_st *CFetch) {
	CURLcode CCode;                                              /* curl result code */
	CFetch->Payload = (char *) calloc(1, sizeof(CFetch->Payload)); /* init payload */

	if (CFetch->Payload == NULL) { return CURLE_FAILED_INIT; }	/* check payload */

	CFetch->Psize = 0; /* init size */
	curl_easy_setopt(CHandle, CURLOPT_URL, URL); /* set url to fetch */
	curl_easy_setopt(CHandle, CURLOPT_WRITEFUNCTION, curl_callback); /* set calback function */
	curl_easy_setopt(CHandle, CURLOPT_WRITEDATA, (void *) CFetch); /* pass fetch struct pointer */
	curl_easy_setopt(CHandle, CURLOPT_USERAGENT, "libcurl-agent/1.0"); /* set default user agent */
	curl_easy_setopt(CHandle, CURLOPT_TIMEOUT, 5); /* set timeout */
	curl_easy_setopt(CHandle, CURLOPT_FOLLOWLOCATION, 1); /* enable location redirects */
	curl_easy_setopt(CHandle, CURLOPT_MAXREDIRS, 1); /* set maximum allowed redirects */
	curl_easy_setopt(CHandle, CURLOPT_REFERER, ref); /* set referer */
	CCode = curl_easy_perform(CHandle); /* fetch the url */
	return CCode;
}

/* call back for curl */
size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb;                                       /* calculate buffer size */
	struct curl_fetch_st *p = (struct curl_fetch_st *) userp;             /* cast pointer to fetch struct */
	p->Payload = (char *) realloc(p->Payload, p->Psize + realsize + 1);    /* expand buffer */
	if (p->Payload == NULL) {	/* check buffer */
		fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback");
		free(p->Payload); /* free buffer */
		return -1;
	}
	memcpy(&(p->Payload[p->Psize]), contents, realsize); /* copy contents to buffer */
	p->Psize += realsize;                                /* set new buffer size */
	p->Payload[p->Psize] = 0;                            /* ensure null termination */
	return realsize;                                    /* return size */
}

/* parse json object */
void parse_json(char *Data) {
	struct json_object *jdata,*data,*data2,*tmp;

	/* reset back to default value */
	Max_Name_Length = Max_Name_Length_Default;
	Flag.Downloading = 0, Flag.Completed = 0;

	jdata = json_tokener_parse(Data);
	json_object_object_get_ex(jdata,"categories",&data);	/* Parse categories list */

	if(data != NULL) {
		categories = malloc(json_object_array_length(data) * sizeof(char*));
		for(int i = 0; i < json_object_array_length(data); ++i) {
			tmp = json_object_array_get_idx(data,i);
			categories[i] = strdup(json_object_get_string(tmp));
			categories_size = i + 1;
			Flag.Categories_Error = 0;
		}

	}else{
		Flag.Categories_Error = 1;
	}

	/* Parse server status */
	json_object_object_get_ex(jdata,"server_state",&data);
	if(data != NULL) {
		server = calloc((sizeof(srv_t)/sizeof(srv_t[0])), sizeof(char*));
		FileSize(&server[0], Column_Width_Default, json_object_get_int64(findobj(data,"alltime_dl")));
		FileSize(&server[1], Column_Width_Default, json_object_get_int64(findobj(data,"alltime_ul")));
		Int2Str(&server[2], Column_Width_Default, (int)json_object_get_int(findobj(data,"dht_nodes")));
		Dbl2Str(&server[3], Column_Width_Default, (double)json_object_get_double(findobj(data,"global_ratio")), 2);
		Flag.Server_Error = 0;
	}else{
		Flag.Server_Error = 1;
	}

	/* Parse torrent */
	json_object_object_get_ex(jdata,"torrents",&data);
	if(data != NULL) {
		torrent_size = json_object_count(data);	/* Get torrent size */
		if(torrent_size != 0) {
			hash = malloc(torrent_size * sizeof(char*));
			name = malloc(torrent_size * sizeof(char*));
			status = malloc(torrent_size * sizeof(char**));
			info = malloc(torrent_size * sizeof(char**));
			int j = 0;
			json_object_object_foreach(data, key2, val2) {
				hash[j] = strdup(key2);
				json_object_object_get_ex(data,key2,&data2);
				name[j] = strdup(json_object_get_string(findobj(data2,"name")));
				Max_Name_Length = (strlen(name[j]) > Max_Name_Length) ? strlen(name[j]) : Max_Name_Length;
				status[j] = calloc((sizeof(sta_t)/sizeof(sta_t[0])), sizeof(char*));
				Dbl2Str(&status[j][0], Column_Width_Default, (double)json_object_get_int(findobj(data2,"dlspeed"))/1000, 2);
				Dbl2Str(&status[j][1], Column_Width_Default, (double)json_object_get_int(findobj(data2,"upspeed"))/1000, 2);
				Dbl2Str(&status[j][2], Column_Width_Default, (float)json_object_get_double(findobj(data2,"progress")) * 100, 2);
				Dbl2Str(&status[j][3], Column_Width_Default, (double)json_object_get_double(findobj(data2,"ratio")), 3);
				/* info list - for multi dimensional array */
				info[j] = calloc((sizeof(inf_t)/sizeof(inf_t[0])), sizeof(char*));
				info[j][0] = strdup(json_object_get_string(findobj(data2,"state")));
				Flag.Downloading = (strcmp(info[j][0],"downloading") == 0 && Flag.Downloading == 0) ? 1 : Flag.Downloading;
				Flag.Update = (HasChng(Flag.Downloading, Flag.Downloading_Ref) == 1 && Flag.Update == 0) ? 1 : 0;
				Flag.Completed = (strcmp(info[j][0],"uploading") == 0 || strcmp(info[j][0],"pausedUP") == 0 && Flag.Completed == 0) ? 1 : Flag.Completed;
				Flag.Update = (HasChng(Flag.Completed, Flag.Completed_Ref) == 1 && Flag.Update == 0) ? 1 : 0;
				FileSize(&info[j][1], Column_Width_Default, json_object_get_int64(findobj(data2,"total_size")));
				FileSize(&info[j][2], Column_Width_Default, json_object_get_int64(findobj(data2,"amount_left")));
				FileSize(&info[j][3], Column_Width_Default, json_object_get_int64(findobj(data2,"uploaded")));
				Time2Str(&info[j][4], json_object_get_int64(findobj(data2,"eta")));
				CombInt2Str(&info[j][5], json_object_get_int(findobj(data2,"num_seeds")), json_object_get_int(findobj(tmp,"num_complete")));
				CombInt2Str(&info[j][6], json_object_get_int(findobj(data2,"num_leechs")), json_object_get_int(findobj(tmp,"num_incomplete")));
				info[j][7] = strdup(json_object_get_string(findobj(data2,"category")));
				j++;
			}
		}
		Flag.Torrent_Error = 0;
	}else{
		Flag.Torrent_Error = 1;
	}
	json_object_put(jdata);
}

/* free all malloc */
void freeAll(void) {
	if(Flag.Categories_Error == 0 && categories_size != 0) {
		for(int i = 0; i < categories_size; ++i) {
			if(categories[i]) {
				free(categories[i]);
			}
		}
		free(categories);
	}
	if(Flag.Server_Error == 0 && (sizeof(srv_t)/sizeof(srv_t[0])) != 0) {
		for(int i = 0; i < (sizeof(srv_t)/sizeof(srv_t[0])); ++i) {
			if(server[i]) {
				free(server[i]);
			}
		}
		free(server);
	}
	if(Flag.Torrent_Error == 0 && torrent_size != 0) {
		for(int j = 0; j < torrent_size; ++j) {
			if(hash[j]) { free(hash[j]); }
			if(name[j]) { free(name[j]); }
			for(int s = 0; s < (sizeof(sta_t)/sizeof(sta_t[0])); ++s) {
				if(status[j][s]) { free(status[j][s]); }
			}
			if(status[j]) { free(status[j]); }
			for(int d = 0; d < (sizeof(inf_t)/sizeof(inf_t[0])); ++d) {
				if(info[j][d]) { free(info[j][d]); }
			}
			if(info[j]) { free(info[j]); }
		}
		if(hash) { free(hash); }
		if(name) { free(name); }
		if(status) { free(status); }
		if(info) { free(info); }
	}
}

/* find json object */
struct json_object * findobj(struct json_object *jobj, const char *key) {
	struct json_object *element;
	json_object_object_get_ex(jobj, key, &element);
	return element;
}

/* Return json object count */
int json_object_count(struct json_object *jobj) {
	int count = 0;
	json_object_object_foreach(jobj, key, val) {
		count++;
	}
	return count;
}

/* curl post pause*/
void post_pause(int Selected) {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/pause");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	post = (char*)malloc((strlen(hash[Selected]) + 6) * sizeof(char));
	sprintf(post,"hash=%s",hash[Selected]);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	if(url_string) {
		free(url_string);
	}
	if(post) {
		free(post);
	}
}

/* curl post pauseAll*/
void post_pauseAll() {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/pauseAll");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	if(url_string) {
		free(url_string);
	}
}

/* curl post resume*/
void post_resume(int Selected) {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/resume");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	post = (char*)malloc((strlen(hash[Selected]) + 6) * sizeof(char));
	sprintf(post,"hash=%s",hash[Selected]);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	free(url_string);
	if(post) {
		free(post);
	}
}

/* curl post resumeAll*/
void post_resumeAll() {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/resumeAll");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	free(url_string);
}

/* curl post category*/
int post_category(int choice, char *cat) {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/setCategory");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	post = (char*)malloc((strlen(hash[choice]) + ((int)strlen(cat)) + 18) * sizeof(char));
	sprintf(post,"hashes=%s&category=%s",hash[choice],cat);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	free(url_string);
	if(post) {
		free(post);
	}
	return 0;
}

/* curl post delete*/
int post_delete(int choice) {
	CURL *curl;
	CURLcode pcurl;
	char *post;
	curl = curl_easy_init();
	char *url_string = URLString(_URL, "/command/delete");
	curl_easy_setopt(curl, CURLOPT_URL, url_string);
	post = (char*)malloc((strlen(hash[choice]) + 6) * sizeof(char));
	sprintf(post,"hashes=%s",hash[choice]);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(post));
	curl_easy_setopt(curl, CURLOPT_REFERER, ref);
	pcurl = curl_easy_perform(curl);
	if(pcurl != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(pcurl));
	}
	curl_easy_cleanup(curl);
	free(url_string);
	if(post) {
		free(post);
	}
	return 0;
}
