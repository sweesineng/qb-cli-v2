#include <time.h>
#include <inttypes.h>

#define eos(s) ((s)+strlen(s))

/* Output current time with format */
void TimeNow(char* output){
  time_t tnow;
  struct tm *time_info;

  time(&tnow);
  time_info = localtime( &tnow );
  strftime(output, 18, "%d %b %I:%M:%S%p", time_info);
}

/* convert to time */
void Time2Str(char **buf, int64_t d) {
   int day, hr, min, sec;
   sec = d%60;
	min = (d/60)%60;
	hr = ((d/60)/60)%24;
	day = ((d/60)/60)/24;
   *buf = calloc(12, sizeof(char)); /* xxxd xxh xxm */
   if(day > 0) {
		sprintf(*buf,"%dd %dh %dm", day, hr, min);
	}
	if((day == 0) && (hr > 0)) {
		sprintf(*buf,"%dh %dm %ds", hr, min, sec);
	}
	if((day == 0) && (hr == 0) && (min > 0)) {
		sprintf(*buf,"%dm %ds", min, sec);
	}
	if((day == 0) && (hr == 0) && (min == 0) && (sec > 0)) {
		sprintf(*buf,"%ds", sec);
	}
}

/* return number of digit */
int dlen(int d) {
	int c = 0;
	if( d == 0) {
		c =1;
	}else{
		while(d != 0) {
			d /= 10;
			++c;
		}
	}
	return c;
}

int dlen64(int64_t d) {
   int c = 0;
	if( d == 0) {
		c =1;
	}else{
		while(d != 0) {
			d /= 10;
			++c;
		}
	}
	return c;
}

/* convert integer byte to human read file size(KB, MB, GB, TB) */
void FileSize(char** buf, int size, int64_t d) {
   int count = 0, decimal = 1;
   double tb = 1000000000000;
	double gb = 1000000000;
	double mb = 1000000;
   double kb = 1000;
   double tmp = (double)d;
   *buf = malloc(size* sizeof(char));

   if(dlen64(d) > 12) {
      snprintf(*buf, size * sizeof(char), "%.*fTB", decimal, tmp/tb);
   }else if(dlen64(d) > 9) {
      snprintf(*buf, size * sizeof(char), "%.*fGB", decimal, tmp/gb);
   }else if(dlen64(d) > 6) {
      snprintf(*buf, size * sizeof(char), "%.*fMB", decimal, tmp/mb);
   }else if(dlen64(d) > 3) {
      snprintf(*buf, size * sizeof(char), "%.*fKB", decimal, tmp/kb);
   }else{
      snprintf(*buf, size * sizeof(char), "%fB", tmp);
   }
}

/* convert int to string */
void Int2Str(char **buf, int size, int d) {
   *buf = malloc(size * sizeof(char));
   snprintf(*buf, size * sizeof(char), "%d", d);
}

/* convert double or float to string with decimal point */
void Dbl2Str(char **buf, int size, double d, int decimal) {
   *buf = malloc(size * sizeof(char));
   snprintf(*buf, size * sizeof(char), "%.*f", decimal, d);
}

/* combine 2 integer with format to string */
void CombInt2Str(char **buf, int d1, int d2) {
   int d1_len = dlen(d1);
   int d2_len = dlen(d2);
   *buf = malloc((d1_len + d2_len + 3) * sizeof(char));
   snprintf(*buf, (d1_len + d2_len + 3) * sizeof(char), "%d(%d)", d1, d2);
}

/* Detect change in variable */
int HasChng(int val, int ref) {
   int result;
   (val != ref) ? ( ref = val, result = 1 ) : ( result = 0 );
   return result;
}
