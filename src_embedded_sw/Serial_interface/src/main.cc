#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "xparameters.h"
#include "xil_printf.h"

/* Basic NN architecture */
#define NB_FEATURES				13
#define NB_HIDDEN_NODES			1
#define NB_CLASSES				4
#define SINGLE_ENDED			1

#define NB_OUTPUT_NODES			(SINGLE_ENDED ? 1 : NB_CLASSES)
#define NB_WEIGHTS1				(NB_FEATURES+1)*NB_HIDDEN_NODES
#define NB_WEIGHTS2				(NB_HIDDEN_NODES+1)*NB_OUTPUT_NODES

#define c_TERM_OUT 				'$'
#define c_ACK 					'%'
#define c_ERR 					'@'
#define c_END					'&'

void get_opcode();
void get_data();

enum implementation_t {SOFTWARE, HARDWARE};
enum session_t {TRAINING, PREDICTION};
enum option_t {GIVEN, RAND, PREV};
enum state_t {Get_opcode, Get_data, Process, Send_result};
implementation_t implementation;
session_t session;
option_t option;
state_t state = Get_opcode;

u32 opcode = 0;
u16 nb_examples = 0, nb_iterations = 0;

u8 *features, *labels = NULL;
u16 *weights = NULL;

int main()
{
	get_opcode();
	get_data();

	/* DEBUG checking */
	xil_printf("%cFirst feature: %d\n",c_TERM_OUT,features[0]);
	xil_printf("%cLast feature: %d\n",c_TERM_OUT,features[NB_FEATURES - 1]);
	if(session == TRAINING)
		xil_printf("%cFirst label: %d\n",c_TERM_OUT,labels[0]);
	if(option == GIVEN) {
		xil_printf("%cFirst weight: %d\n",c_TERM_OUT,weights[0]);
		xil_printf("%cLast weight: %d\n",c_TERM_OUT,weights[NB_WEIGHTS1 + NB_WEIGHTS2 - 1]);
	}

	outbyte(c_END);
	return 0;
}

void get_opcode()
{
	u32 i;
	for(i = 0; i < 4; i++) {
		opcode |= u8(inbyte()) << i*8;
	}
	outbyte(c_ACK);

	switch((opcode >> 19) & 1) {
	case 0:
		implementation = SOFTWARE;
		break;
	default:
		implementation = HARDWARE;
		break;
	}

	switch((opcode >> 18) & 1) {
	case 0:
		session = TRAINING;
		break;
	default:
		session = PREDICTION;
		break;
	}

	switch((opcode >> 16) & 0x3) {
	case 0:
		option = GIVEN;
		break;
	case 1:
		option = RAND;
		break;
	default:
		option = PREV;
		break;
	}

	nb_iterations = (opcode >> 20) & 0xFFF;
	nb_examples = opcode & 0xFFFF;
}

void get_data()
{
	u32 i,j;

	features = new u8 [nb_examples*NB_FEATURES]; //(u8*) malloc(nb_examples*NB_FEATURES*sizeof(u8));
	if(session == TRAINING)
		labels = new u8[nb_examples]; //(u8*) malloc(nb_examples*sizeof(u8));
	if(option == GIVEN)
		weights = new u16[NB_WEIGHTS1 + NB_WEIGHTS2]; //(u16*) malloc((NB_WEIGHTS1 + NB_WEIGHTS2)*sizeof(u16));

	if(option == GIVEN) {
		for(i = 0; i < (NB_WEIGHTS1 + NB_WEIGHTS2); i++) {
			weights[i] = u16(inbyte()) | (u16(inbyte()) << 8);
			outbyte(c_ACK);
		}
	}

	for(i = 0; i < nb_examples; i++) {
		for(j = 0; j < NB_FEATURES; j++) {
			features[i*NB_FEATURES + j] = u8(inbyte());
		}
		if(session == TRAINING)
			labels[i] = u8(inbyte());

		outbyte(c_ACK);
	}
}
