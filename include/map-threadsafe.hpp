#ifndef MAP_THREAD_SAFE
#define MAP_THREAD_SAFE

#include <map>
#include <thread>
#include <mutex>
#include <shared_mutex>

namespace std
{
    namespace threadsafe
    {
        /*
        Concurrent Map prototype, it allows concurrent reads and safe writes.

        */
        template<class K,class V>
        class map
        {
            std::map<K,V> internal_map;

            std::shared_mutex mutex;

            public:
                map(){}
                map(const std::map<K,V> &m) : internal_map(m){}
                map(const std::threadsafe::map<K,V> &m) = delete;

                bool get(const K& key,V& output)
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    auto it = internal_map.find(key);
                    if (it == internal_map.end())
                    {
                        return false;
                    }
                    else{
                        output = it->second;
                    	return true;
		    }
                }


                bool getTail(V& output)
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    if(!internal_map.empty()){
                        output = internal_map.rbegin()->second;
                        return true;
                    }
                    return false;
                }

                bool getHead(V& output)
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    if(!internal_map.empty()){
                        output = internal_map.begin()->second;
                        return true;
                    }
                    return false;
                }

                bool getAndEraseHead(V& output)
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    if(!internal_map.empty()){
                        auto iter = internal_map.begin();
                        output = iter->second;
                        internal_map.erase(iter);
                        return true;
                    }
                    return false;
                }

                bool eraseHead()
                {
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    if(!internal_map.empty()){
                        auto iter = internal_map.begin();
                        internal_map.erase(iter);
                        return true;
                    }
                    return false;
                }
            
                bool insert(const K & key,V & value){
                    std::unique_lock<std::shared_mutex> lock(mutex);
                    return internal_map.insert(std::make_pair(key,value)).second;
                }
            
                size_t erase(const K & key){
                    std::unique_lock<std::shared_mutex> lock(mutex);
                    return internal_map.erase(key);
                }

                size_t size(){
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    return internal_map.size();
                }

                size_t sizeNoThreadSafe(){
                    return internal_map.size();
                }

                bool empty(){
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    return internal_map.empty();
                }
                
                bool contains(const K& key){
                    std::shared_lock<std::shared_mutex> lock(mutex);
                    auto it = internal_map.find(key);
                    return it == internal_map.end();
                }

		void clear(){
			std::unique_lock<std::shared_mutex> lock(mutex);
			internal_map.clear();
		}                

                V putIfAbsent(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			auto it = internal_map.find(key);
			if (it == internal_map.end()){
				internal_map.insert(std::make_pair(key,value));
				return value;
			}
			else
				return it->second;
		}

		template<typename Iterator>
		Iterator getValues(Iterator begin,Iterator end){
			std::shared_lock<std::shared_mutex> lock(mutex);
			auto it = internal_map.begin();
			while (begin != end && it != internal_map.end()){
				*begin = *it;
				it++;
				begin++;
			}

			return begin;
		}

		bool remove(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			auto it = internal_map.find(key);
			if (it == internal_map.end() || it->second != value)
				return false;
			else
			{
				internal_map.erase(it);
				return true;
			}
		}

		bool replace(const K& key,const V& value){
			std::unique_lock<std::shared_mutex> lock(mutex);
			
			auto it = internal_map.find(key);
			if (it != internal_map.end()){
				it->second = value;
				return true;
			}
			return false;
		}

                bool insertOrReplace(const K& key,const V& value){
                    std::unique_lock<std::shared_mutex> lock(mutex);

                    auto it = internal_map.find(key);
                    if (it != internal_map.end()){
                        it->second = value;
                        return false;
                    }
                    else{
                        return internal_map.insert(std::make_pair(key,value)).second;
                    }
                }

            bool insertOrReplace(const K& key,const V& newValue, V& oldValue){
                std::unique_lock<std::shared_mutex> lock(mutex);

                auto it = internal_map.find(key);
                if (it != internal_map.end()){
                    oldValue = it->second;
                    it->second = newValue;
                    return false;
                }
                else{
                    return internal_map.insert(std::make_pair(key,newValue)).second;
                }
            }

		bool replace(const K& key,V& oldValue,V& newValue){
			std::unique_lock<std::shared_mutex> lock(mutex);
			
			auto it = internal_map.find(key);
			if (it != internal_map.end() && it->second == oldValue){
				it->second = newValue;
				return true;
			}
			return false;
		}
        };
    }
}

#endif
