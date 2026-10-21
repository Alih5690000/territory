#include <vector>
#include <iostream>

struct Team{
    char color;
    int id;
};

const Team TEAM_DEFAULT=Team{'.', 0};

struct Pixel {
    int x,y;
    Team team;
    float hp;
    bool Invade(Pixel* p){
        if (hp>0 && p->team.id!=team.id){
            p->team=team;
            hp-=p->hp/10;
            p->hp=0;
            return true;
        }
        return false;
    }
};

typedef std::vector<std::vector<Pixel>> Map;

std::vector<Pixel*> GetNeighbors(Map& m, const Pixel& p){
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    std::vector<Pixel*> neighbors;
    for (int i = 0; i < 4; ++i) {
        int nx = p.x + dx[i];
        int ny = p.y + dy[i];
        if (ny >= 0 && ny < m.size() &&
                nx >= 0 && nx < m[ny].size()) {
            neighbors.push_back(&m[ny][nx]);
        }
    }
    return neighbors;
}

class Territory{
    public:
    Team team;
    Map& map;
    float hp=20;
    float power=20.f;
    std::vector<Pixel*> ps;
    std::vector<Pixel*> getBorders(){
        std::vector<Pixel*> borders;
        for (const auto& p : ps) {
            auto neighbors = GetNeighbors(map, *p);
            for (const auto& n : neighbors) {
                if (n->team.id != team.id) {
                    borders.push_back(p);
                    break;
                }
            }
        }
        return borders;
    }
    void invade(int team){
        int invaded=0;
        auto bs=getBorders();
        for (auto i:bs){
            auto ns=GetNeighbors(map, *i);
            if (team==-1){
                std::vector<Pixel*> available;
                for (auto j : ns) {
                    if (j->team.id != this->team.id)
                        available.push_back(j);
                }
                if (!available.empty()) {
                    auto j = available[rand() % available.size()];
                    i->hp=power;
                    if(i->Invade(j)){
                        ps.push_back(j);
                        power=i->hp;
                        invaded++;
                    }
                }
            }
            else{
                for (auto j:ns){
                    if (j->team.id==team){
                        i->hp=power;
                        if(i->Invade(j)){
                            ps.push_back(j);
                            power=i->hp;
                            invaded++;
                        }
                    }
                }
            }
        }
        hp+=invaded/5.f;
    }
    void rest(float am){
        power+=am;
    }
    Territory(Map& ma, std::vector<Pixel*> p={}):map(ma){
        ps=p;
    }
};

Map CreateMap(int w, int h){
    Map res;
    for (int i=0;i<h;i++){
        std::vector<Pixel> a;
        for (int j=0;j<w;j++){
            a.push_back(Pixel{j,i,TEAM_DEFAULT, 20});
        }
        res.push_back(a);
    }
    return res;
}

void PrintMap(const Map& m){
    for (auto i:m){
        for (auto j:i){
            std::cout<<j.team.color<<' ';
        }
        std::cout<<std::endl;
    }
}

int main(){
    srand(time(NULL));
    Map map=CreateMap(20, 20);
    map[0][0]=Pixel{0, 0, {'#', 1}, 20};
    Territory t(map, {&map[0][0]});
    t.team={'#', 1};
    for (int k=0;k<3;k++){
        for (int i=0;i<100;i++)
            t.invade(-1);
        t.rest(5);
    }
    PrintMap(map);
}
