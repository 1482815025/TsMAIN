#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>

class Observer {
public:
    virtual void update(std::pair<std::vector<unsigned long>, std::string> id_length_dir, unsigned char* payload) = 0;
    virtual ~Observer() {}
};

#endif /* OBSERVER_HPP */