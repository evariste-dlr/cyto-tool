#include "lib/qmathstools.h"

void
QMathsTools::normalize(QVector<double>& vector, const double& value){
    // Recherche du maximum
    double max;
    if (vector.size() > 0){
        max = vector[0];
    }
    for (int i=1; i<vector.size(); i++){
        max = (vector[i] > max) ? vector[i] : max;
    }

    // Normalisation
    for (int i=0; i<vector.size(); i++){
        vector[i] *= value / max;
    }
}


double
QMathsTools::median(const QVector<double>& a){
    int n = a.size();
    return kthSmallest(a, ((n & 1) ? (n / 2) : (n / 2)-1));
}


double
QMathsTools::kthSmallest(QVector<double> a, int k)
{
    int i,j,l,m;
    double x;
    double tmp;

    l = 0;
    m = a.size()-1;
    while (l < m) {
        x = a[k];
        i = l;
        j = m;
        do {
            while (i < a.size() && a[i] < x) i++;
            while (j >= 0 && x < a[j]) j--;
            if (i <= j){
                tmp = a[i];
                a[i] = a[j];  a[j] = tmp;
                i++; j--;
            }
        } while (i <= j);
        if (j < k) l=i;
        if (k < i) m=j;
    }
    return a[k];
}


double
QMathsTools::mean(const QVector<double>& samples){
    double sum = .0;
    for (int i=0; i<samples.size(); i++){
        sum += samples[i];
    }
    return sum / samples.size();
}

double
QMathsTools::variance(const QVector<double>& samples){
    double m = mean(samples);
    double sum = .0;
    for (int i=0; i<samples.size(); i++){
        sum += (m - samples[i]) * (m - samples[i]);
    }
    return sum / samples.size();
}




