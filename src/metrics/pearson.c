#include "pearson.h"

extern const int SYMBOL_COUNT;
extern const int ONE_MINUTE_MS;
extern const int BUFFER_SIZE;
extern volatile int interrupted;
extern char *symbols[];
extern long long current_timestamp_ms();
extern MvAvgBuffer **mv_buf;

maxCorr max_pearson_correlation(MvAvgBuffer *mv_buf1, MvAvgBuffer *mv_buf2) {

    int size1 = mv_buf1->count, size2 = mv_buf2->count;
    if(size1 < 8 || size2 < 8){
        maxCorr result;
        result.ts = 0;
        result.value = -1.0;
        return result;
    }
    
    double *mv1 = malloc(8 * sizeof(double));
    double *mv2 = malloc(8 * sizeof(double));

    for(int i=0; i<8; i++){
        mv1[i] = mvavg_buffer_get(mv_buf1, size1-1-i).value;
        mv2[i] = mvavg_buffer_get(mv_buf2, size2-1-i).value;
    }
    double max_correlation = calculate_pearson_correlation(mv1, mv2, 8);
    long long ts = mvavg_buffer_get(mv_buf2, size2-1).ts;

    for(int i=1; i<=size2-8; i++){
        for(int j=0; j<8; j++){
            mv2[j] = mvavg_buffer_get(mv_buf2, size2-1-i-j).value;
        }
        double correlation = calculate_pearson_correlation(mv1, mv2, 8);
        if(correlation > max_correlation){
            max_correlation = correlation;
            ts = mvavg_buffer_get(mv_buf2, size2-1-i).ts;
        }
    }

    maxCorr result;
    result.ts = ts;
    result.value = max_correlation;
    return result;
}

double calculate_pearson_correlation(double *x, double *y, int n) {
    double sum_x = 0.0, sum_y = 0.0;
    double sum_x_sq = 0.0, sum_y_sq = 0.0;
    double p_sum = 0.0;

    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i];
        sum_x_sq += x[i] * x[i];
        sum_y_sq += y[i] * y[i];
        p_sum += x[i] * y[i];
    }

    double num = p_sum - (sum_x * sum_y / n);
    double den = sqrt((sum_x_sq - (sum_x * sum_x / n)) * (sum_y_sq - (sum_y * sum_y / n)));

    if (den == 0) return 0.0;

    return num / den;
}

void *pearson_thread_func(void *arg){
    long long base_time = *(long long *)arg;
    base_time = base_time - (base_time % ONE_MINUTE_MS) + ONE_MINUTE_MS; //Next full minute
    base_time += 8*ONE_MINUTE_MS; //8 minutes later

    while (!interrupted) {
        long long now = current_timestamp_ms();
        if (now < base_time) {
            usleep((base_time - now) * 1000);
        }

        for(int i=0; i<SYMBOL_COUNT; i++){
            double resultCorr = -1.0;
            long long resultTs = 0;
            int resultSymbol = i;

            for(int j=0; j<SYMBOL_COUNT; j++){
                if(i == j) continue;

                maxCorr result = max_pearson_correlation(mv_buf[i], mv_buf[j]);
                if(result.value > resultCorr && result.ts > 0){
                    resultCorr = result.value;
                    resultTs = result.ts;
                    resultSymbol = j;
                }
            }

            write_pearson(symbols[i], resultTs, symbols[resultSymbol], resultCorr);
        }
        base_time += ONE_MINUTE_MS;
    }

    return NULL;
}

