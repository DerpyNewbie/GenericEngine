#pragma once

/// <summary>
/// 基底クラスのshared_from_thisから、派生クラスのshared_ptrを取得できるようにするクラスです。
/// </summary>
template <class Base>
class enable_shared_from_base : public std::enable_shared_from_this<Base>
{
protected:
    /// <summary>
    /// shared_from_this()をDerivedにCastして取得します。
    /// </summary>
    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::dynamic_pointer_cast<Derived>(this->shared_from_this());
    }
};