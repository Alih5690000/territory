#include <vector>
#include <iostream>
#include <algorithm>
#include <windows.h>
#include "json.hpp"
#include <vector>
#include "main.hpp"


struct Team{
    char color;
    int id;
};

const Team TEAM_DEFAULT=Team{'.', 0};

struct Pixel {
    int x,y;
    Team team;
    std::vector<Pixel*>& t;
    float attack;
    float defense;
    Pixel(int x_, int y_, Team t, float att, float def,
        std::vector<Pixel*>& tt):
        x(x_),y(y_),team(t),attack(att),defense(def),t(tt){}
    bool Invade(Pixel* p){
        if (attack>0 && p->team.id!=team.id){
            p->team=team;
            attack-=p->defense;
            p->attack=0;
            p->t.erase(
                std::remove(p->t.begin(), 
                p->t.end(), p), p->t.end());
            return true;
        }
        return false;
    }
};

typedef std::vector<std::vector<Pixel>> MMap;

std::vector<Pixel*> GetNeighbors(MMap& m, const Pixel& p){
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

std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> roads;

class Territory{
    public:
    Team team;
    MMap& map;
    float Money=0.f;
    float AttackBoost=1.f;
    float DefenseBoost=1.f;
    float MaxAttack=10;
    float MaxDefense=1;
    float attack=10;
    float defense=1.f;
    float oldDefense=defense;
    std::vector<Pixel*> ps;
    std::vector<Pixel*> getBorders(){
        std::vector<Pixel*> borders;
        for (const auto& p : ps) {
            if (p->team.id != team.id)
                continue;
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
    void invade(Territory ttt){
        float left=0.f;
        bool first=true;
        do{
        bool avai=false;
        //std::cout<<"Left is "<<left<<std::endl;
        //std::cout<<"Atack is "<<attack<<std::endl;
        int team=ttt.team.id;
        int invaded=0;
        auto bs=getBorders();
        //std::cout<<"Borders size is "<<bs.size()<<std::endl;
        ttt.InitDefense();
        for (auto i:bs){
            auto ns=GetNeighbors(map, *i);
            if (team==-1){
                std::vector<Pixel*> available;
                for (auto j : ns) {
                    if (j->team.id != this->team.id)
                        available.push_back(j);
                }
                if (!available.empty()) {
                    avai=true;
                    if (first)
                        i->attack=attack*AttackBoost+left;
                    else i->attack=left;
                    float budget1=(first ? attack : left);
                    bool sucess=false;
                    float sum=0;
                    int val=0;
                    std::vector<Pixel*> visited;
                    while(budget1>0){
                        if (visited.size()==available.size()
                            || i->attack<=0.f) break;
                        Pixel* j;
                        while(true){
                            j = available[rand() % available.size()];
                            if (std::find(visited.begin(), 
                                visited.end(), j) == visited.end()){
                                    visited.push_back(j);
                                    break;
                            }   
                        }
                        //std::cout<<"j defense is "<<j->defense<<std::endl;
                        if(i->Invade(j) && 
                            std::find(ps.begin(), ps.end(), j) == ps.end()){
                            //std::cout<<"Invasion"<<std::endl;
                            ps.push_back(j);
                            invaded++;
                            sucess=true;
                            budget1-=j->defense;
                            sum+=j->defense;
                            val++;
                        }
                        else{
                            if (j->team.id!=this->team.id)
                                CancelDefense(j);
                        }
                    }
                    //std::cout<<"Attack is "<<i->attack<<std::endl;
                    left=std::max(0.f, i->attack);
                    if (sucess && first) attack-=sum/val;
                }
            }
            else{
                std::vector<Pixel*> available;
                for (auto j : ns) {
                    if (j->team.id != team)
                        available.push_back(j);
                }
                if (!available.empty()) {
                    avai=true;
                    if (first)
                        i->attack=attack*AttackBoost+left;
                    else i->attack=left;
                    float budget1=(first ? attack : left);
                    bool sucess=false;
                    float sum=0;
                    int val=0;
                    std::vector<Pixel*> visited;
                    while(budget1>0){
                        if (visited.size()==available.size()
                            || i->attack<=0.f) break;
                        Pixel* j;
                        while(true){
                            j = available[rand() % available.size()];
                            if (std::find(visited.begin(), 
                                visited.end(), j) == visited.end()){
                                    visited.push_back(j);
                                    break;
                            }   
                        }
                        //std::cout<<"j defense is "<<j->defense<<std::endl;
                        if(i->Invade(j) && 
                            std::find(ps.begin(), ps.end(), j) == ps.end()){
                            //std::cout<<"Invasion"<<std::endl;
                            ps.push_back(j);
                            invaded++;
                            sucess=true;
                            budget1-=j->defense;
                            sum+=j->defense;
                            val++;
                        }
                        else{
                            if (j->team.id!=this->team.id)
                                CancelDefense(j);
                        }
                    }
                    //std::cout<<"Attack is "<<i->attack<<std::endl;
                    left=std::max(0.f, i->attack);
                    if (sucess && first) attack-=sum/val;
                }
            }
        }
        first=false;
        if (!avai) break;
        } while (left>0.f);
    }
    void UpdateStats(){
        MaxAttack=ps.size();
        MaxDefense=ps.size();
    }
    float part;
    void InitDefense(){
        auto bs=getBorders();
        if (bs.empty()) return;
        part=defense/bs.size();
        for (auto i:bs){
            i->defense+=defense*DefenseBoost;
        }
        oldDefense=defense;
        defense=0;
    }
    void CancelDefense(Pixel* p){
        p->defense-=part;
        defense+=part;
    }
    void rest(float am){
        defense=std::min(defense+am, MaxDefense);
        attack=std::min(attack+am, MaxAttack);
    }
    Territory(MMap& ma, std::vector<Pixel*> p={}):map(ma){
        ps=p;
    }
};

std::vector<Pixel*> EMPTY={};
MMap EMPTY_MAP;

MMap CreateMap(int w, int h){
    MMap res;
    for (int i=0;i<h;i++){
        std::vector<Pixel> a;
        for (int j=0;j<w;j++){
            a.push_back(Pixel{j,i,TEAM_DEFAULT, 10, 10, 
                EMPTY});
        }
        res.push_back(a);
    }
    return res;
}

void ReadyMap(MMap& m){
    for (int i=0;i<roads.size();i++){
        auto& road=roads[i];
        auto& p1=road.first;
        auto& p2=road.second;
        for (int x=std::min(p1.first, p2.first);
            x<=std::max(p1.first, p2.first);x++){
            m[p1.second][x].team={'R', 0};
        }
        for (int y=std::min(p1.second, p2.second);
            y<=std::max(p1.second, p2.second);y++){
            m[y][p1.first].team={'R', 0};
        }
    }
}

int PrintMap(const MMap& m){
    int won=m[0][0].team.id;
    for (int I=0;I<m.size();I++){
        for (int J=0;J<m[I].size();J++){
            std::cout<<m[I][J].team.color<<' ';
            if (m[I][J].team.id!=won && 
                m[I][J].team.id!=0) won=-1;
        }
        std::cout<<std::endl;
    }
    return won;
}

const auto EMPTY_LANDS=[](MMap& m){
    auto t=Territory(m);
    t.team.id=-1;
    return t;
};

void reset_console_cursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hConsole, pos);
}

int main(int argc, char* argv[]){
    if (argc>1 && std::string(argv[1])=="view"){
        DataBase db("victories.db");
        Map main=db.ReadMapEx("main");
        std::vector<Ellement> wins=main.Get("wins").GetVec();
        std::cout<<"Wins: ";
        for (auto i:wins){
            std::cout<<i.GetInt()<<' ';
        }
        std::cout<<std::endl;
        return 0;
    }
    srand(time(NULL));
    DataBase db("victories.db");
    Object cfg=OpenJson("config.json");
    Object plr1_cfg=cfg["plr1"].get<Object>();
    Object plr2_cfg=cfg["plr2"].get<Object>();
    MMap map=CreateMap(20, 20);
    Territory t(map, {&map[0][0]});
    char plr1_clr=plr1_cfg["color"].get<std::string>()[0];
    map[0][0].team={plr1_clr, 1};
    t.team={plr1_clr, 1};
    float plr1_attack=plr1_cfg["attack"].get<float>();
    t.AttackBoost=plr1_attack;
    Territory tt(map, {&map[0][19]});
    char plr2_clr=plr2_cfg["color"].get<std::string>()[0];
    map[0][19].team={plr2_clr, 2};
    tt.AttackBoost=plr2_cfg["attack"].get<float>();
    tt.team={plr2_clr, 2};
    float rest1=plr1_cfg["rest"].get<float>();
    float rest2=plr2_cfg["rest"].get<float>();
    for (;;){
        t.invade(EMPTY_LANDS(map));
        tt.invade(EMPTY_LANDS(map));
        t.UpdateStats();
        tt.UpdateStats();
        t.rest(rest1);
        tt.rest(rest2);
        ReadyMap(map);
        int won=PrintMap(map);
        if (won!=-1){
            if (db.attrs.find("main")==db.attrs.end()){
                std::vector<Ellement> wins;
                wins.push_back(Ellement{won});
                Map<Ellement> main({Ellement{"wins", wins}});
                db.AddMapEx("main", main);
                db.WriteTo("victories.db");
                std::cout<<"Player "<<won<<" won!"<<std::endl;
                return 0;
            }
            else{
                Map main=db.ReadMapEx("main");
                std::vector<Ellement> wins=main.Get("wins").GetVec();
                wins.push_back(Ellement{won});
                main.Set("wins", Ellement{"wins", wins, true});
                db.AddMapEx("main", main);
                db.WriteTo("victories.db");
                std::cout<<"Player "<<won<<" won!"<<std::endl;
                return 0;
            }
        }
        reset_console_cursor();
    }
    /*t.rest(5);
    tt.rest(5);*/
    PrintMap(map);
}
