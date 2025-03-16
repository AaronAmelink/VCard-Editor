#include "VCParser.h"
#include "VCHelper.h"

int main(int argc, char *argv[]) {
    Card *card1 = NULL;
    Card *card2 = NULL;
    VCardErrorCode response = createCard(argv[1], &card1);

    if (response != OK) {
        print(errorToString(response));
    }

    writeCard(NULL, NULL);

    if (card1 != NULL) {

        const char* fileName = argv[2];
        writeCard(fileName, card1);
        VCardErrorCode response = createCard(argv[2], &card2);
        print(errorToString(response));
        print(cardToString(card2));
        printf("%d\n", validateCard(card2));
        deleteCard(card1);
        deleteCard(card2);
    }


    return 1;
}