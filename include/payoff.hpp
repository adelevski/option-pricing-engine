#pragma once


class payoff 
{
public:
    virtual ~payoff() = default;
    virtual double operator()(const double& S) const = 0;
};


class call_payoff : public payoff 
{
public:
    call_payoff(const double& K);
    double operator()(const double& S) const override;

private:
    double K_;
};


class put_payoff : public payoff 
{
public:
    put_payoff(const double& K);
    double operator()(const double& S) const override;

private:
    double K_;
};
