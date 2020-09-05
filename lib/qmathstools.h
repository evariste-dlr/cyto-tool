#ifndef QMATHSTOOLS_H
#define QMATHSTOOLS_H

#include <QVector>

/**
 *  Regrouppement de fonctions utilitaires mathématiques
 *  utilisant les structures de données Qt (en particulier QVector)
 */
namespace QMathsTools{

    /**
     * Normaliser un vecteur avec pour valeur maximale value.
     * T doit être un type arithmétique.
     * @param [inout] vector  le vecteur à normaliser
     * @param [in]  value     la valeur maximale du nouveau vecteur
     */
    void normalize(QVector<double>& vector, const double& value = 1.0);


    /**
     * Trouve la médiane de l'ensemble a avec la fonction kthSmallest.
     * @see kthSmallest
     */
    double median(const QVector<double>& a);


    /**
     *  Find the kth smallest element in the array.
     *  use the median() function defined previoulsy to get the median.
     * @author Wirth, Niklaus
     * @param a  array of elements
     * @param k  rank k
     * @return the kth smallest element in a
     *
     * Reference:
     *
     *               Author: Wirth, Niklaus
     *                Title: Algorithms + data structures = programs
     *            Publisher: Englewood Cliffs: Prentice-Hall, 1976
     * Physical description: 366 p.
     *               Series: Prentice-Hall Series in Automatic Computation
     */
    double kthSmallest(QVector<double> a, int k);


    /**
     * @brief Moyenne de l'ensemble
     * @param samples    tableau de valeurs
     * @return la moyenne arithmétique des valeurs de samples
     */
    double mean(const QVector<double>& samples);

    /**
     * @brief Variance de l'ensemble
     * @param samples   tableau de valeurs
     * @return la variance
     */
    double variance(const QVector<double>& samples);
}

#endif // QMATHSTOOLS_H





