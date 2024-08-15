#include <iostream>
#include <cstdlib>
#include <cstring>

class N {
    private:
        int     value;
        char    annotation[108];
        
    public:

        N(int nbr): value(nbr) {}

        int operator+ (N &n) {
            return this->value += n.value;
        }

        int operator- (N &n) {
            return this->value -= n.value;
        }

        void setAnnotation(char *annotation) {
            memcpy(this->annotation, annotation, strlen(annotation));
        }
};

int main(int ac, char **av) {
    if (ac < 2)
        exit(1);
    
    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(av[1]);

    return (*a + *b);
}
