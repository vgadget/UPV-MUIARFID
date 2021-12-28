import scipy.spatial as spatial
from scipy.stats import entropy
from scipy.stats import pearsonr
import math

import numpy as np

__author__ = 'fgarcia'

"""
Modulo con funciones relativas al calculo de distancias
"""


class DistanceError(Exception):
    pass


def symmetric_kl(v_query, v_audio):
    return (entropy(v_query, v_audio) + entropy(v_audio, v_query)) / 2

def call_pearson(x,y):
    return (1-pearsonr(x,y)[0])/2

def get_distance_matrix(audio1, audio2, distance='cos'):
    """
    Calcula la matriz de distancias segun la funcion definida
    :param audio1: Matriz de caracteristicas del audio 1
    :param audio2: Matriz de caracteristicas del audio 1
    :param distance: Distancia utilizada en el calculo de la matriz.
        cos  - Distancia coseno
        euc  - Distancia euclidea
        corr - Distancia de correlacion
        dot  - Producto escalar
        kl   - Divergencia Kullback-Leibler
    :return: Matriz de distancias entre ambos audios.
    """
    if distance == 'cos':
        return spatial.distance.cdist(audio1, audio2, 'cosine')
    elif distance == 'euc':
        return spatial.distance.cdist(audio1, audio2, 'euclidean')
    elif distance == 'corr':
        return spatial.distance.cdist(audio1, audio2, 'correlation')
    elif distance == 'dot':
        return np.dot(audio1, audio2.T)
    elif distance == 'logdot':
        return -np.log(np.dot(audio1, audio2.T))
    elif distance == 'kl':
        return spatial.distance.cdist(audio1, audio2, entropy)
    elif distance=='pearson':
        return spatial.distance.cdist(audio1, audio2, call_pearson)
    else:
        raise DistanceError('Invalid distance')


def compact_distance_matrix(dist_matrix, window=3, step=2):
    # Devuelve una matriz suavizada de la matriz de distancias
    # En este caso, el suavizado es la norma de las distancias
    comp_matrix = np.zeros((dist_matrix.shape[0] // 2, dist_matrix.shape[1] // 2))
    for i in comp_matrix.shape[0]:
        for j in comp_matrix.shape[1]:
            comp_matrix[i, j] = dist_matrix[i:i+window, j:j+window].sum() / window**2
    return comp_matrix
