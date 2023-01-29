#include <iostream>
#include "person.pb.h"

using namespace std;

int main()
{
    PERSON::Person p;

    p.set_name("bluebird");

    if (p.has_name()) {
        cout << "Name : " << *p.mutable_name() << endl;
    } else {
        cout << "Name is not defined\n";
    }

    return 0;
}
