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

    void update_scene_stats(const scene &s);
    void update_benchmark_stats(const benchmark &b);
    void update_render_info(const scene &s);

    std::vector<std::string> get() const;
    
private:
    std::string scene_stats;
    std::string benchmark_stats;
    std::string render_stats;
};

#endif
