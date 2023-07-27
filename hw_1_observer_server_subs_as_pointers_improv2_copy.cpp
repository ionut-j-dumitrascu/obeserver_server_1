#include <iostream>
#include <string>
#include <vector>
#include <map>

class IOstation{
    public:
        virtual ~IOstation(){};
        // virtual void - notify_all, notify_IOdevice, add_IOdevice, remove_IOdevice, ~
        // 
};

class IOdevice{
    public:
        virtual ~IOdevice(){};
        virtual int my_number(){return 0;};
        virtual void on_screen_notify(int){};
        // virtual void - notify_in, send_message
};

class Station : public IOstation{
    public:
        virtual ~Station(){
        }
        
        void save_message(int nr_dest, int nr_sender, std::string message_in){
            // save received message from another sub to respective destination
            std::map<IOdevice *, std::vector<std::string>>::iterator subs_iterator;

            bool valid_sub=false;
			// Find IODevice with phone num nr_dest
            for(subs_iterator=subs_msgs_map.begin(); subs_iterator!=subs_msgs_map.end(); subs_iterator++)
                if((subs_iterator->first)->my_number()==nr_dest){
					// Found IODevice and stored in (subs_iterator->first)
                    valid_sub=true;
                    (subs_iterator->second).push_back(message_in);

					// pass (subs_iterator->first) as parameter
                    notify_IOdevice(nr_dest,nr_sender, message_in);
                    break;  
                }
            if(!valid_sub)
                std::cout<<"\n Destination number not found";
        }

        void notify_all(){
            //notify all subs with message_received broadcasted through station
            // iterator through all
        }
		
        void notify_IOdevice(int nr_dest, int nr_sender, std::string new_message){
            //notify destination_sub with message_received sent to station. can be done virtual and used in a p2p_station type
            std::map<IOdevice *, std::vector<std::string>>::iterator subs_iterator;

            std::cout<<"\n Server: New message from: "<<nr_sender<<" to "<<nr_dest;
            
			// call direct on_screen_notify fro dest - no need to search again
			for(subs_iterator=subs_msgs_map.begin(); subs_iterator!=subs_msgs_map.end(); subs_iterator++)
                if((subs_iterator->first)->my_number()==nr_dest){
                    (subs_iterator->first)->on_screen_notify(nr_sender);
                    std::cout<<"\n Text: "<<new_message;
                    break;
                }
        }
		
        void add_IOdevice(IOdevice *device_in){
            // add device_in to subs_msgs_map
            
            auto result = subs_msgs_map.find(device_in);
            if(subs_msgs_map.empty()){     // no entries found
                std::cout<<"\n !!! No entries found !!! Adding first number: "<<device_in->my_number();
                subs_msgs_map[device_in] = std::vector<std::string>{};
            }
            else if (result != subs_msgs_map.end()){         // entry found -> do nothing
                std::cout<<"\n !!! Entry found !!! Nr ."<<device_in->my_number()<<" already registered";
            }
            else if(result == subs_msgs_map.end()){     // entry not found -> add number
                std::cout<<"\n !!! Entry "<<device_in->my_number()<<" not found !!! Nr. "<<device_in->my_number()<<" will be registered ";
                subs_msgs_map[device_in] = std::vector<std::string>{};
                //std::cout<<device_in->my_number();
            }
        }

        void remove_IOdevice(int nr){
            std::map<IOdevice *, std::vector<std::string>>::iterator subs_iterator;
            
            subs_iterator = subs_msgs_map.begin();
            auto iter_next = subs_iterator;
            
            while(iter_next != subs_msgs_map.end()){
                ++iter_next;
                if((iter_next->first)->my_number()==nr && iter_next!=subs_msgs_map.end()){
                    std::cout<<"\n Sub "<<(iter_next->first)->my_number()<<" will be deleted";
                    subs_msgs_map.erase(iter_next);
                    break;
                }
                subs_iterator = iter_next;

            }

            std::cout<<"\n Remaining subs: ";
            all_subs();

        }


        // void remove_IOdevice(int nr){
        //     std::map<IOdevice *, std::vector<std::string>> subs_msgs_map;

        //     for(auto iter_next : subs_msgs_map){
        //         if((iter_next.first)->my_number()==nr){
        //             std::cout<<"\n Sub "<<(iter_next.first)->my_number()<<" will be deleted";
        //             subs_msgs_map.erase(iter_next);
        //             std::cout<<"\n Remaining subs: ";
        //             all_subs();


        //     }
        // }}

        void retrieve_all_messages(IOdevice *device_){
            int j=0;
            auto result = subs_msgs_map.find(device_);
            if (result != subs_msgs_map.end()){
                auto messages = (*result).second;
                for(std::string i: messages){
                    std::cout<<"\n Mesaj["<<j<<"]: "<<i;
                    j++;
                }
            }
            else{
               std::cout<<"\n Number "<<device_->my_number()<<" not found / removed"; 
            }
        }

        void all_subs(){
            // return no. of subbed devices
            std::map<IOdevice *, std::vector<std::string>>::iterator subs_iterator;

            std::cout<<"\n all subs:";
            for(subs_iterator=subs_msgs_map.begin(); subs_iterator!=subs_msgs_map.end(); subs_iterator++)
                std::cout<<"\n"<<(subs_iterator->first)->my_number();
        }

    private:
        // message
        std::string message_received;
        std::map<IOdevice *, std::vector<std::string>> subs_msgs_map; // din IOdevice* luam sub_number
};

class Device : public IOdevice{
    public:
        Device(Station &station, int new_sub_number) : 
			used_station(station), 
			sub_number(new_sub_number)
		{

            this->used_station.add_IOdevice(this);
            
        }
        virtual ~Device(){
            used_station.remove_IOdevice(this->sub_number);
            std::cout<<"\n Device removed \n";
        }
        void on_screen_notify(int sender){
            // used by Station:notify_all and Station:notify_IOdevice
            std::cout<<"\n Phone - "<<my_number()<<": "<<" Mesaj primit de la "<<sender;
        }
        void send_message(int dest_nr, std::string mesaj){
            // using Station:save_message, Station:notify_IOdevice
            used_station.save_message(dest_nr, my_number(), mesaj);
        }

        int my_number(){
            return this->sub_number;
        }

    private:
        Station &used_station;
        int id_number, sub_number;
};

void example_runs(){

    Station *server_ex = new Station;
    
    Device *subscriber1 = new Device(*server_ex, 123);
    Device *subscriber2 = new Device(*server_ex, 456);
    Device *subscriber3 = new Device(*server_ex, 789);
    Device *subscriber4 = new Device(*server_ex, 456);
    Device *subscriber5 = new Device(*server_ex, 222);
    
    server_ex->all_subs();

    subscriber1->send_message(789, "hello there");
    subscriber1->send_message(789, "hello there2");
    subscriber1->send_message(789, "hello there3");

    subscriber1->send_message(222, "hello there4");
    subscriber1->send_message(222, "hello there5");
    subscriber1->send_message(222, "hello there6");

    server_ex->retrieve_all_messages(subscriber3);

    server_ex->remove_IOdevice(222);

    server_ex->retrieve_all_messages(subscriber5);

    subscriber3->send_message(222, "check message");
}

int main(){
    
    example_runs();
    return 0;

}