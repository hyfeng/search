#ifndef EXE_LAYER_CONTROL_H
#define EXE_LAYER_CONTROL_H
#include <memory>
#include <string>
#include <vector>
#include <map>


namespace da {

//执行逻辑控制，执行单元分层管理
template <typename TYPE>
class ExeLayerController {
    //TYPE是基类，管理的类的都需要继承的东西, 实际存储的是类的实例指针
public:
    ExeLayerController() : _count(0) {}
    /**
     * @brief 返回当前可以执行的所有类
     * @brief 返回入度为0的所有类
     * @return 需要执行策略的个数
     * */
    int pop_head_layer(std::vector<std::shared_ptr<TYPE> >& layer);
    /*
     * @brief: 添加策略实例
     * @param pre_name 是前驱
     * @如果前驱不存在则失败
     * */
    bool add_strategy(std::shared_ptr<TYPE>& strategy,
            const std::string& name,
            const std::vector<std::string>& pre_name);
    /*
     * @brief: 添加策略实例
     * 无前驱
     * */
    bool add_strategy(std::shared_ptr<TYPE>& strategy, 
            const std::string& name);

private:
    std::vector<int> _degree;   //所有实例的入度
    std::vector<std::shared_ptr<TYPE>> _strategy; //所有的策略实例
    std::vector<std::vector<int>> _edge; //后向边,指向下一个策略
    int _count; //实例的数量
    std::map<std::string, int> _name2idx; //实例名字到下标的映射
};

template <typename TYPE>
int ExeLayerController<TYPE>::pop_head_layer(std::vector<std::shared_ptr<TYPE> >& layer) {
    layer.clear();
    int cnt = 0;
    std::vector<int> res;
    for (int i = 0; i < _count; ++i) {
        if (_degree[i] == 0) {
            res.emplace_back(i);
        }
    }
    cnt = res.size();
    for (int i : res) {
        layer.emplace_back(_strategy[i]);
        --_degree[i];
        for (int j : _edge[i]) {
            --_degree[j];
        }
    }
    return cnt;
}

template <typename TYPE>
bool ExeLayerController<TYPE>::add_strategy(
        std::shared_ptr<TYPE>& strategy,
        const std::string& name,
        const std::vector<std::string>& pre_name) {
    if (pre_name.empty()) {
        return add_strategy(strategy, name);
    }
    if (_name2idx.find(name) != _name2idx.end()) {
        return false;
    }
    _strategy.emplace_back(strategy);
    _degree.emplace_back(pre_name.size());
    _edge.emplace_back(std::vector<int>());
    _name2idx[name] = _count;
    ++_count;
    for (auto&& pname : pre_name) {
        auto it = _name2idx.find(pname);
        if (it != _name2idx.end()) {
            int idx = it->second;
            _edge[idx].emplace_back(_count - 1);
        } else {
            return false;
        }
    }
    return true;
}

template<typename TYPE>
bool ExeLayerController<TYPE>::add_strategy(
        std::shared_ptr<TYPE>& strategy,
        const std::string& name) {
    if (_name2idx.find(name) != _name2idx.end()) {
        return false;
    }
    _strategy.emplace_back(strategy);
    _degree.emplace_back(0);
    _edge.emplace_back(std::vector<int>());
    _name2idx[name] = _count;
    ++_count;
    return true;
}

}///~namespace da
#endif
