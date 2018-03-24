#ifndef CONV_PARAMS_H_
#define CONV_PARAMS_H_

//winograd tile/filter size
#define N 2
#define R 3
#define M 4
	//DO NOT CHANGE UNTIL NEXT VERSION

//strassen macroalgorithm size
#define T 4 //tiles to elaborate at once
#define C 4 //image channels
#define K 4 //filter banks

//kernels [KxC] * in [CxT] = out [KxT];

//struttura dati
	//matrice di immagini/kernel
//procedura:
	//PRE: trasforma KxC filtri (la MacroMatrice di filtri)
	//selezionare CxT tile di input
	//trasformarli, memorizzarli
	//per ogni coppia [i,j] in [K,T]
		//distribuire il pixel [i,j] dei tile/filtri trasformati sulla matrice di strassen
		//moltiplicare la matrice via strassen
		//mettere i risultati nel pixel [i,j] del risp. tile di output
	//si ottengono KxT tile output
	//antitrasformare i tile e metterli nella MacroMatrice output

#define AS_STRING(X) #X
#define STRING(X) AS_STRING(X)
//#define CHAIN(P1,P2,P3,P4) P1 x P2 _ P3 x P4
#define CHAIN(P1,P2) P1 x P1 _ P2 x P2

//#define CODE_FILENAME_TF_INPUT STRING(/Users/Dario/Desktop/winograd/ADAPTIVE_v4/transformCode/CHAIN(N,R) .txt)
#define FILENAME_TF_INPUT STRING(wg_fragm/in CHAIN(N,R) .txt)
#define FILENAME_TF_KERN STRING(wg_fragm/fil CHAIN(N,R) .txt)
#define FILENAME_ATF_OUT STRING(wg_fragm/out CHAIN(N,R) .txt)

#endif /* CONV_PARAMS_H_ */
