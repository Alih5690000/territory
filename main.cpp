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
    float attack;
    float defense;
    Pixel(int x_, int y_, Team t, float att, float def):
        x(x_),y(y_),team(t),attack(att),defense(def){}
    bool Invade(Pixel* p){
        if (attack>0 && p->team.id!=team.id){
            p->team=team;
            attack-=p->defense/10;
            p->attack=0;
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
    float power=20.f;
    float attack=10;
    float defense=10.f;
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
                    i->attack=attack;
                    i->defense=defense;
                    if(i->Invade(j)){
                        ps.push_back(j);
                        attack=i->attack;
                        invaded++;
                    }
                }
            }
            else{
                for (auto j:ns){
                    if (j->team.id==team){
                        i->attack=attack;
                        i->defense=defense;
                        if(i->Invade(j)){
                            ps.push_back(j);
                            attack=i->attack;
                            invaded++;
                        }
                    }
                }
            }
        }
        attack+=invaded/5.f;
    }
    void rest(float am){
        defense+=am;
        attack+=am;
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
            a.push_back(Pixel{j,i,TEAM_DEFAULT, 10, 10});
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
    map[0][0]=Pixel{0, 0, {'#', 1}, 0, 10};
    Territory t(map, {&map[0][0]});
    t.team={'#', 1};
    for (int k=0;k<3;k++){
        for (int i=0;i<100;i++)
            t.invade(-1);
        t.rest(5);
    }
    PrintMap(map);
}
