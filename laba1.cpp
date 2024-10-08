#include <random>
#include <iostream>
#include <fstream>
#include <memory>

class AnyDice
{
public:
    AnyDice() {}
    virtual unsigned int roll()
    {
        return 0;
    }
};

class Dice : public AnyDice
{
public:
    Dice(unsigned int max, unsigned int seed) : max(max), dstr(1, max), reng(seed) {}

    unsigned int roll() override
    {
        return dstr(reng);
    }

private:
    unsigned int max;
    std::uniform_int_distribution<unsigned int> dstr;
    std::default_random_engine reng;
};

class ThreeDicePool : public AnyDice
{
public:
    ThreeDicePool(std::shared_ptr<AnyDice> d1, std::shared_ptr<AnyDice> d2, std::shared_ptr<AnyDice> d3) : dice_1(std::move(d1)), dice_2(std::move(d2)), dice_3(std::move(d3))
    {
    }
    unsigned int roll()
    {
        return dice_1->roll() + dice_2->roll() + dice_3->roll();
    }

private:
    std::shared_ptr<AnyDice> dice_1;
    std::shared_ptr<AnyDice> dice_2;
    std::shared_ptr<AnyDice> dice_3;
};

class PenaltyDice : public AnyDice
{
public:
    PenaltyDice(std::shared_ptr<AnyDice> d) : d(d) {}
    virtual unsigned int roll() override
    {
        return std::min(d->roll(), d->roll());
    }
    // virtual unsigned int penalty(){

    // }
private:
    std::shared_ptr<AnyDice> d;
};

class BonusDice : public AnyDice
{
public:
    BonusDice(std::shared_ptr<AnyDice> d) : d(d) {}
    // virtual unsigned int bonus(){
    //     return std::max(d.roll(), d.roll());
    // }
    virtual unsigned int roll() override
    {
        return std::max(d->roll(), d->roll());
    }

private:
    std::shared_ptr<AnyDice> d;
};

class DoubleDice : public AnyDice
{
public:
    DoubleDice(std::shared_ptr<AnyDice> d) : dice(std::move(d)) {}
    unsigned int roll() override
    {
        BonusDice bonusDice(dice);
        PenaltyDice penaltyDice(dice);
        return bonusDice.roll() + penaltyDice.roll();
    }

private:
    std::shared_ptr<AnyDice> dice;
};

//multiple inheritance for DoubleDice
// class DoubleDice : public BonusDice, public PenaltyDice
// {
// public:
//     DoubleDice(std::shared_ptr<Dice> d) : {}
//     unsigned int roll() override
//     {
//         return BonusDice::roll() + PenaltyDice::roll();
//     }
// };

double expected_value(std::shared_ptr<AnyDice> d, unsigned int number_of_rolls = 1)
{
    auto accum = 0llu;
    for (unsigned int cnt = 0; cnt != number_of_rolls; ++cnt)
        accum += d->roll();
    return static_cast<double>(accum) / static_cast<double>(number_of_rolls);
}
double value_probability(unsigned int value, std::shared_ptr<AnyDice> d, unsigned int number_of_rolls = 1)
{
    unsigned int counter = 0;
    for (unsigned int cnt = 0; cnt != number_of_rolls; ++cnt)
    {
        if (d->roll() == value)
            ++counter;
    }
    return 1.0 * counter / (number_of_rolls * 1.0);
}

int main()
{
    // auto d1=std::make_shared<Dice>(6, 4);
    // auto d2=std::make_shared<Dice>(6, 2);
    // auto d3=std::make_shared<Dice>(6, 1);
    // auto three_dice=std::make_shared<ThreeDicePool>(d1, d2, d3);
    // auto b_dice=std::make_shared<BonusDice>(d1);
    // auto p_dice=std::make_shared<PenaltyDice>(d1);
    // auto d_dice=std::make_shared<DoubleDice>(d1);
    //std::cout<<value_probability(3, d_dice, 100);
    //std::cout<<expected_value(d1, 100)<<' '<<expected_value(three_dice, 100);
    //std::cout<<value_probability(3, d1, 100)<<' '<<value_probability(3, b_dice, 100)<<' '<<value_probability(3, p_dice, 100)<<' '<<value_probability(3, three_dice, 100);
    
    
    std::ofstream out;          
    out.open("data_dice.csv");      
    if (out.is_open()) out<<"Dice"<<std::endl;
    unsigned int number_of_rolls=1000, max=100;
    auto dice=std::make_shared<Dice>(max, 25);
    for(unsigned int i=1;i<=max;i++)
        if (out.is_open()) out << value_probability(i, dice, number_of_rolls)<< std::endl;
    out.close();

    out.open("data_bonus_dice.csv");      
    if (out.is_open()) out<<"BonusDice"<<std::endl;
    auto bonus_dice=std::make_shared<BonusDice>(dice);
    for(unsigned int i=1;i<=max;i++)
         if (out.is_open()) out << value_probability(i, bonus_dice, number_of_rolls)<< std::endl;
    out.close();

    out.open("data_penalty_dice.csv");      
    if (out.is_open()) out<<"PenaltyDice"<<std::endl;
    auto penalty_dice=std::make_shared<PenaltyDice>(dice);
    for(unsigned int i=1;i<=max;i++)
         if (out.is_open()) out << value_probability(i, penalty_dice, number_of_rolls)<< std::endl;
    out.close();

    out.open("data_three_pool_dice.csv");      
    if (out.is_open()) out<<"TreePoolDice"<<std::endl;
    max=6;
    auto d1=std::make_shared<Dice>(max, 3);
    auto d2=std::make_shared<Dice>(max, 5);
    auto d3=std::make_shared<Dice>(max, 2);
    auto three_dice=std::make_shared<ThreeDicePool>(d1, d2, d3);
    for(unsigned int i=1;i<=18;i++)
         if (out.is_open()) out << value_probability(i, three_dice, number_of_rolls)<< std::endl;
    out.close();

    out.open("data_double_dice.csv");     
    max=100; 
    if (out.is_open()) out<<"DoubleDice"<<std::endl;
    auto double_dice=std::make_shared<DoubleDice>(dice);
    for(unsigned int i=1;i<=max;i++)
         if (out.is_open()) out << value_probability(i, double_dice, number_of_rolls)<< std::endl;
    out.close();
}