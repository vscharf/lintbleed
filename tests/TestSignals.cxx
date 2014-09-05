#include <iostream>
#include <vector>
#include <boost/signals2.hpp>
#include <memory>

template <typename ValueT>
class Container {
public:
  typedef ValueT value_type;
  typedef std::function<void(const value_type&)> signal_type;
  boost::signals2::connection connect_insert(const signal_type& s) {
    return insert_signal.connect(s);
  }
  
  void insert(const value_type& v) {
    std::cout << "insert " << v << std::endl;
    insert_signal(v);
  }

private:
  boost::signals2::signal<void(const value_type&)> insert_signal;
};

struct Listener {
private:
  boost::signals2::scoped_connection c;

public:
  template <typename Container>
  Listener(Container& cont) : x(x_) {
    c = cont.connect_insert([this](const typename Container::value_type& v) {this->handle_insert(v); });
  }
};

template <typename Cont>
class SomeFunc {
  boost::signal2::scoped_connection;
public:
  SomeFunc(Cont& c) : m_c(c) {}
  
};

int main() {
  Container<int> cont;

  Listener l(5, cont);
  Listener* l2 = new Listener(10, cont);

  cont.insert(5);

  delete l2;

  std::vector<int> vvv(16, 42);

  cont.insert(66);
}
