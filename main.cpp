#include <vector>

struct Team{
    char color;
    int id;
};

struct Pixel {
    int x,y;
    Team team;
    int hp;
    void Invade(Pixel* p){
        p->team=team;
        hp-=p->hp/10;
    }
};

typedef std::vector<std::vector<Pixel>> Map;

std::vector<Pixel> GetNeighbors(const Map& m, const Pixel& p){
    const int dx[3] = {1, -1, 0};
    const int dy[3] = {0, 1, -1};
    std::vector<Pixel> neighbors;
    for (int i = 0; i < 3; ++i) {
        int nx = p.x + dx[i];
        int ny = p.y + dy[i];
        if (nx >= 0 && nx < m.size() && ny >= 0 && ny < m[0].size()) {
            neighbors.push_back(m[nx][ny]);
        }
    }
    return neighbors;
}

class Territory{
    Team team;
    Map& map;
    std::vector<Pixel> ps;
    std::vector<Pixel> getBorders(){
        std::vector<Pixel> borders;
        for (const auto& p : ps) {
            auto neighbors = GetNeighbors(map, p);
            for (const auto& n : neighbors) {
                if (n.team.id != team.id) {
                    borders.push_back(p);
                    break;
                }
            }
        }
        return borders;
    }
};
