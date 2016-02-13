#ifndef BUILD_FACTORY_H
#define BUILD_FACTORY_H

#include "dawgdic/dawg-builder.h"
#include "dawgdic/guide-builder.h"
#include "ISerializable.h"
#include "Dictionary.h"
#include <string>
#include <memory>
namespace dawg {

template<typename T>
class BasicDawg;

template<class T,class U>
class Dictionary;

/**
 * @brief The BuildFactory - template factory for building
 * dictionaries. It is usefull, when you constract Dictionary for
 * the first time - from values.
 * There are some limitations on T, achieved by using
 *  C++11 <type_traits>. T must be POD - class or must
 * implement ISerializable interface.
 */
template<typename T>
class BuildFactory : public dawgdic::DawgBuilder {

private:
    /**
     * @brief dawg from dawgdic library representing connections
     * in graph;
     */
    dawgdic::Dawg dawg;
    /**
     * @brief data that will be stored in Dictionary
     */
    std::vector<T> data;

public:
    /**
     * @brief BuildFactory - Default constructor, checks T's limitations using static_assert
     */
    BuildFactory():dawgdic::DawgBuilder(){
        static_assert(std::is_base_of<ISerializable, T>::value || std::is_pod<T>::value, "Type T must be POD or derived from ISerializable!");
    }
    /**
     * @brief insert key-value pair to factory. You should insert keys
     * is alphabetical order only.
     * @param str - key
     * @param value - value
     * @return success of operations.
     */
    bool insert(std::string str,const T& value){
        int index = data.size();
        if(dawgdic::DawgBuilder::Insert(str.c_str(),str.length(),index)){
            data.push_back(value);
            return true;
        }
        return false;
    }
    /**
     * @brief build - build Dictionary from inserted values.
     * After building, all preveosly inserted data will be cleared.
     * You should fill factory again.
     * @return shared_pointer to created dictionary.
     * Don't care abour second template parameter arg,
     * it's type_traits issue.
     */
    std::shared_ptr<Dictionary<T,void>> build(){
        DawgBuilder::Finish(&dawg);
        Dictionary<T,void>  *result = new Dictionary<T,void>();
        if(dawgdic::DictionaryBuilder::Build(dawg,result->dict)) {
            if(dawgdic::GuideBuilder::Build(dawg,*result->dict,result->guide)) {
                result->setData(data);
                result->init();
                dawg.Clear();
                data.clear();
                return std::shared_ptr<Dictionary<T,void>>(result);
            }
        }
        delete result;
        return nullptr;
    }
};
}
#endif
