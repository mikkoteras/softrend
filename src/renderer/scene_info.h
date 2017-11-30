#ifndef SCENE_INFO_H
#define SCENE_INFO_H

#include <string>
#include <vector>

class benchmark;
class scene;

class scene_info {
public:
    scene_info();
    ~scene_info();

    void update(scene &parent);
    std::vector<std::string> get() const;
    
private:
    std::string scene_stats;
    std::string benchmark_stats;
    std::string render_stats;
};

#endif
