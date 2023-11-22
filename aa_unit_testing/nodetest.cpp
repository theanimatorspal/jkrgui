//#include <Extra/Node.hpp>
//#include <algorithm>
//#include <iostream>
//#include <numeric>
//#include <span>
//
//auto ___Add(std::vector<DataType> inType)
//{
//    auto Result = 0;
//    for (auto& u : inType) {
//        Result += std::get<int>(u);
//    }
//    return DataType(Result);
//}
//
//auto ___Sub(std::vector<DataType> inType)
//{
//    using namespace std;
//    return DataType(get<int>(inType.front()) - get<int>(inType.back()));
//}
//
//int main()
//{
//    using namespace App;
//    Node Number1(25);
//    Node Number2(35);
//
//    Node Number3(25);
//    Node Number4(35);
//
//    Node Number5(25);
//    Node Number6(35);
//
//    Node Add(___Add, 2);
//
//    Node Sub(___Sub, 2);
//
//    Node AddTotal(___Add, 2);
//
//    Node::ConnectNode(&Number1, &Add, 0);
//    Node::ConnectNode(&Number2, &Add, 1);
//
//    Node::ConnectNode(&Number3, &Sub, 0);
//    Node::ConnectNode(&Number4, &Sub, 1);
//
//    Node::ConnectNode(&Sub, &AddTotal, 0);
//    Node::ConnectNode(&Add, &AddTotal, 1);
//
//    Node Add1(Add);
//
//    Node Sub2(Sub);
//       
//    Node::ConnectNode(&Number5, &Add1, 0);
//    Node::ConnectNode(&Number6, &Add1, 1);
//
//    Node::ConnectNode(&Add1, &Sub2, 0);
//    Node::ConnectNode(&AddTotal, &Sub2, 1);
//
//
//    Add1.Cook();
//    Add.Cook();
//    Sub.Cook();
//    AddTotal.Cook();
//    Add.Cook();
//    Sub.Cook();
//
//    Add1.Cook();
//
//    std::cout << std::get<int>(Sub2.Cook());
//}
int main ( )
{

}