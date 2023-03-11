#pragma once

#include "Core/Definitions.h"

NAMESPACE_START(Gear)

namespace DesignPatterns
{
	namespace _CreationalPatterns
	{
		namespace _Factory
		{
			namespace _SimpleFactory
			{
				class IProduct
				{
				public:
					virtual ~IProduct() = default;
					virtual void function() = 0;
				};

				class ProductA : public IProduct
				{
				public:
					void function() override final
					{
					}
				};

				class ProductB : public IProduct
				{
				public:
					void function() override final
					{
					}
				};

				class Factory
				{
				public:
					template<class T>
					IProduct* CreateProduct()
					{
						return new T();
					}

					IProduct* CreateProductA()
					{
						return new ProductA();
					}

					IProduct* CreateProductB()
					{
						return new ProductB();
					}
				};
			}

			namespace _AbstractFactory
			{
				class IProductA : public _SimpleFactory::IProduct
				{
				public:
					virtual ~IProductA() = default;
				};

				class IProductB : public _SimpleFactory::IProduct
				{
				public:
					virtual ~IProductB() = default;
				};

				class ProductA : public IProductA
				{
				public:
					void function() override final
					{
					}
				};

				class ProductB : public IProductB
				{
				public:
					void function() override final
					{
					}
				};

				class IFactory
				{
				public:
					virtual ~IFactory() = default;
					virtual _SimpleFactory::IProduct* CreateProduct() = 0;
				};

				class FactoryA : public IFactory
				{
				public:
					_SimpleFactory::IProduct* CreateProduct() override final
					{
						return new ProductA();
					}
				};

				class FactoryB : public IFactory
				{
				public:
					_SimpleFactory::IProduct* CreateProduct() override final
					{
						return new ProductB();
					}
				};
			};
		}

		namespace _Singleton
		{
		}

		namespace _Builder
		{
			class Product
			{
			public:
				void BuildPartA(const std::string& partA)
				{
					m_PartA = partA;
				}

				void BuildPartB(const std::string& partB)
				{
					m_PartB = partB;
				}
			protected:
				std::string m_PartA;
				std::string m_PartB;
			};

			class IBuilder
			{
			public:
				virtual ~IBuilder() = default;
				virtual void BuildPartA() = 0;
				virtual void BuildPartB() = 0;

				Product* Get()
				{
					return &m_Product;
				}
			protected:
				Product m_Product;
			};

			class BuilderA : public IBuilder
			{
			public:
				void BuildPartA() override final
				{
					m_Product.BuildPartA("1");
				}
				void BuildPartB() override final
				{
					m_Product.BuildPartB("2");
				}
			};

			class BuilderB : public IBuilder
			{
			public:
				void BuildPartA() override final
				{
					m_Product.BuildPartA("3");
				}
				void BuildPartB() override final
				{
					m_Product.BuildPartB("4");
				}
			};

			class BuilderDierctor
			{
			public:
				Product* Get(IBuilder* builder)
				{
					assert(builder);
					builder->BuildPartA();
					builder->BuildPartB();
					return builder->Get();
				}
			};
		}

		namespace _Prototype
		{
			class IPrototype
			{
			public:
				virtual ~IPrototype() = default;
				virtual IPrototype* Clone() const = 0;
			};

			class Prototype : public IPrototype
			{
			public:
				Prototype(const Prototype& other)
				{
				}

				IPrototype* Clone() const override final
				{
					return new Prototype(*this);
				}
			};
		}
	}

	namespace _StructuralPatterns
	{
		namespace _Bridge
		{
			class IImplementation
			{
			public:
				virtual ~IImplementation() = default;
				virtual void Implement() = 0;
			};

			class ImplementationA : public IImplementation
			{
			public:
				void Implement() override final
				{
				}
			};

			class ImplementationB : public IImplementation
			{
			public:
				void Implement() override final
				{
				}
			};

			class IAbstraction
			{
			public:
				IAbstraction(IImplementation* implementation) 
					: m_Implementation(implementation)
				{
				}
				virtual ~IAbstraction() = default;
				virtual void Implement() = 0;
			protected:
				IImplementation* m_Implementation = nullptr;
			};

			class Abstraction : public IAbstraction
			{
			public:
				using IAbstraction::IAbstraction;
				void Implement() override final
				{
					m_Implementation->Implement();
				}
			};
		}

		namespace _Adapter
		{
			class Target
			{
			public:
				virtual ~Target() = default;
				virtual void Request()
				{
				}
			};

			class Adaptee
			{
			public:
				void OtherRequest()
				{
				}
			};

			class ClassAdapter : public Target, private Adaptee
			{
			public:
				void Request()
				{
					OtherRequest();
				}
			};

			class ObjectAdapter : public Target
			{
			public:
				ObjectAdapter(Adaptee* adaptee)
					: m_Adaptee(adaptee)
				{
				}

				void Request()
				{
					m_Adaptee->OtherRequest();
				}
			private:
				Adaptee* m_Adaptee;
			};
		}

		namespace _Decorator
		{
			class IComponent
			{
			public:
				virtual ~IComponent() = default;
				virtual void Operation()
				{
				}
			};

			class ComponentA : public IComponent
			{
			public:
				void Operation() override final
				{
					IComponent::Operation();

					/// OtherOperation
				}
			};

			class IDecorator : public IComponent
			{
			public:
				IDecorator(IComponent* component)
					: m_Comp(component)
				{
				}

				void Operation() override
				{
					m_Comp->Operation();
				}
			protected:
				IComponent* m_Comp;
			};

			class DecoratorA : public IDecorator
			{
			public:
				using IDecorator::IDecorator;

				void Operation() override final
				{
					IDecorator::Operation();
				}
			};

			class DecoratorB : public IDecorator
			{
			public:
				using IDecorator::IDecorator;

				void Operation() override final
				{
					IDecorator::Operation();
				}
			};

			void Function()
			{
				IComponent* comp = new DecoratorB(new DecoratorA(new ComponentA()));
				comp->Operation();
			}
		}

		namespace _Composite
		{
			class IComponent
			{
			public:
				virtual ~IComponent() = default;
				virtual IComponent* GetChild(int) { return nullptr; }

				virtual void Add(IComponent*) {}
				virtual void Operation() = 0;
			};

			class Component : public IComponent
			{
			public:
				void Operation() override final
				{
				}
			};

			class Composite : public IComponent
			{
			public:
				IComponent* GetChild(int index) override final
				{
					return m_Components[index];
				}

				void Add(IComponent* comp)
				{
					m_Components.emplace_back(comp);
				}

				void Operation() override final
				{
					for (auto comp : m_Components)
					{
						comp->Operation();
					}
				}
			protected:
				std::vector<IComponent*> m_Components;
			};
		}

		namespace _FlyWeight
		{
			class IFlyWeight
			{
			public:
				virtual ~IFlyWeight() = default;
				virtual void Operation() = 0;

				uint32_t GetState() { return m_State; }
			protected:
				IFlyWeight(uint32_t state)
					: m_State(state)
				{
				}
				uint32_t m_State = ~0u;
			};

			class FlyWeight : public IFlyWeight
			{
			public:
				friend class FlyWeightFactory;
				using IFlyWeight::IFlyWeight;
				void Operation() override final
				{
				}
			};

			class FlyWeightFactory
			{
			public:
				IFlyWeight* GetFlyWeightObject(uint32_t state)
				{
					for (auto flyweight : m_Objects)
					{
						if (flyweight->GetState() == state)
						{
							return flyweight;
						}
					}

					auto flyweight = new FlyWeight(state);
					m_Objects.emplace_back(flyweight);

					return flyweight;
				}
			protected:
				std::vector<IFlyWeight*> m_Objects;
			};
		}

		namespace _Facade
		{
			class SubSystemA
			{
			public:
				void Operation()
				{
				}
			};

			class SubSystemB
			{
			public:
				void Operation()
				{
				}
			};

			class Facade
			{
			public:
				Facade(SubSystemA* a, SubSystemB* b)
					: m_SubSystemA(a)
					, m_SubSystemB(b)
				{
				}

				void OperationWrapper()
				{
					m_SubSystemA->Operation();
					m_SubSystemB->Operation();
				}
			protected:
				SubSystemA* m_SubSystemA = nullptr;
				SubSystemB* m_SubSystemB = nullptr;
			};
		}

		namespace _Proxy
		{
			class ISubject
			{
			public:
				virtual ~ISubject() = default;
				virtual void Request() = 0;
			};

			class Subject : public ISubject
			{
			public:
				void Request() override final
				{
				}
			};

			class Proxy : public ISubject
			{
			public:
				Proxy()
					: m_Subject(new Subject())
				{
				}

				void Request() override final
				{
					m_Subject->Request();
				}
			protected:
				ISubject* m_Subject = nullptr;
			};
		}
	}

	namespace _BehavioralPatterns
	{
		namespace _Template
		{
			class ITemplate
			{
			public:
				virtual ~ITemplate() = default;
				void Method()
				{
					PrimitiveOperationA();
					PrimitiveOperationB();
				}
			protected:
				virtual void PrimitiveOperationA() = 0;
				virtual void PrimitiveOperationB() = 0;
			};

			class Template : public ITemplate
			{
			public:
				void PrimitiveOperationA() override final
				{
				}

				void PrimitiveOperationB() override final
				{
				}
			};

			void Function()
			{
				ITemplate* tem = new Template();
				tem->Method();
			}
		}

		namespace _Strategy
		{
			class IStrategy
			{
			public:
				virtual ~IStrategy() = default;
				virtual void Operation() = 0;
			};

			class StrategyA : public IStrategy
			{
			public:
				void Operation() override final
				{
				}
			};

			class StrategyB : public IStrategy
			{
			public:
				void Operation() override final
				{
				}
			};

			class Context
			{
			public:
				Context(IStrategy* strategy)
					: m_Strategy(strategy)
				{
				}

				void Operation()
				{
					m_Strategy->Operation();
				}
			protected:
				IStrategy* m_Strategy = nullptr;
			};
		}

		namespace _State
		{
			class IState
			{
			public:
				virtual void Operation() = 0;
			};

			class StateA : public IState
			{
			public:
				void Operation() override final
				{
				}
			};

			class StateB : public IState
			{
			public:
				void Operation() override final
				{
				}
			};

			class Context
			{
			public:
				void ChangeState(IState* state)
				{
					m_CurState = state;
					m_CurState->Operation();
				}
			protected:
				IState* m_CurState = nullptr;
			};
		}

		namespace _Observer
		{
			class IObserver
			{
			public:
				virtual void Update(class Subject*) = 0;
			protected:
				uint32_t m_State = ~0u;
			};

			class Subject
			{
			public:
				uint32_t GetState() const
				{
					return m_State;
				}

				void ChangeState(uint32_t state)
				{
					m_State = state;
				}

				void Notify()
				{
					for (auto observer : m_Observers)
					{
						observer->Update(this);
					}
				}

				void RegisterObserver(IObserver* observer)
				{
					m_Observers.emplace_back(observer);
				}
			protected:
				std::vector<IObserver*> m_Observers;
				uint32_t m_State = ~0u;
			};

			class Observer : public IObserver
			{
			public:
				void Update(Subject* target) override final
				{
					m_State = target->GetState();
				}
			};
		}

		namespace _Memento
		{
			class Memento
			{
			public:
				friend class Originator;
			protected:
				Memento(int state)
					: m_State(state)
				{
				}

				int m_State = 0;
			};

			class Originator
			{
			public:
				Memento* GetMemento()
				{
					if (!m_Memento)
					{
						m_Memento = new Memento(m_State);
					}
					return m_Memento;
				}

				int State() const
				{
					return m_State;
				}

				void Save()
				{
					m_State = m_Memento->m_State;
				}

				void Restore()
				{
					m_Memento->m_State = m_State;
				}
			protected:
				Memento* m_Memento = nullptr;
				int m_State = 0;
			};
		}

		namespace _Mediator
		{
			class IColleague
			{
			public:
				IColleague(class IMediator* mediator, int state)
					: m_Mediator(mediator)
					, m_State(state)
				{
				}

				int State() const
				{
					return m_State;
				}

				void SetState(int state)
				{
					m_State = state;
				}
			protected:
			private:
				class IMediator* m_Mediator = nullptr;
				int m_State = 0;
			};

			class ColleagueA : public IColleague
			{
			public:
				using IColleague::IColleague;
			};

			class ColleagueB : public IColleague
			{
			public:
				using IColleague::IColleague;
			};

			class IMediator
			{
			public:
				void Append(IColleague* colleague)
				{
					m_Colleagues.emplace_back(colleague);
				}

				virtual void Dispatch(IColleague* sender) = 0;
			protected:
				std::vector<IColleague*> m_Colleagues;
			};

			class Mediator : public IMediator
			{
			public:
				void Dispatch(IColleague* sender) override final
				{
					assert(sender);
					for (auto col : m_Colleagues)
					{
						if (sender != col)
						{
							col->SetState(sender->State());
						}
					}
				}
			};
		}

		namespace _Command
		{
			class Reciver
			{
			public:
				void Action()
				{
				}
			};

			class ICommand
			{
			public:
				virtual void Execute() = 0;
			};

			class Command : ICommand
			{
			public:
				Command(Reciver* reciver)
					: m_Reciver(reciver)
				{
				}

				void Execute() override final
				{
					m_Reciver->Action();
				}
			protected:
				Reciver* m_Reciver = nullptr;
			};

			class Invoker
			{
			public:
				Invoker(ICommand* command)
					: m_Command(command)
				{
				}

				void Invoke()
				{
					m_Command->Execute();
				}

			protected:
				ICommand* m_Command = nullptr;
			};
		}

		namespace _Visitor
		{
			class IVisitor
			{
			public:
				virtual void Visit(class IElement* element) = 0;
			};

			class IElement
			{
			public:
				virtual void Visit(IVisitor* visitor) = 0;
				virtual void Action() = 0;
			};

			class VisitorA : public IVisitor
			{
			public:
				void Visit(IElement* element)
				{
					element->Action();
				}
			};

			class VisitorB : public IVisitor
			{
			public:
				void Visit(IElement* element)
				{
					element->Action();
				}
			};

			class ElementA : public IElement
			{
			public:
				void Visit(IVisitor* visitor) override final
				{
					visitor->Visit(this);
				}
			};

			class ElementB : public IElement
			{
			public:
				void Visit(IVisitor* visitor) override final
				{
					visitor->Visit(this);
				}
			};
		}

		namespace _ChainOfResponsibility
		{
			class IHandler
			{
			public:
				virtual void HandleRequest() = 0;
				void SetSuccessor(IHandler* successor)
				{
					m_Succrssor = successor;
				}
			protected:
				IHandler* m_Succrssor = nullptr;
			};

			class HandlerA : public IHandler
			{
			public:
				void HandleRequest() override final
				{
					/// Operation A:
					if (m_Succrssor)
					{
						m_Succrssor->HandleRequest();
					}
				}
			};

			class HandlerB : public IHandler
			{
			public:
				void HandleRequest() override final
				{
					/// Operation B:
					if (m_Succrssor)
					{
						m_Succrssor->HandleRequest();
					}
				}
			};
		}

		namespace _Iterator
		{
			class IAggregate
			{
			public:
				virtual class IIterator* CreateIterator() = 0;
			};

			class Aggregate : public IAggregate
			{
			public:
				Aggregate(uint32_t size)
					: m_Size(size)
				{
					m_List = new uint32_t[size]();
				}

				uint32_t Size() const
				{
					return m_Size;
				}

				uint32_t operator[](uint32_t index) const
				{
					assert(index < m_Size);
					return m_List[index];
				}
			protected:
				uint32_t m_Size = 0;
				uint32_t* m_List = nullptr;
			};

			class IIterator
			{
			public:
				virtual void First() = 0;
				virtual void Next() = 0;
				virtual bool End() const = 0;
				virtual uint32_t Item() const = 0;
			};

			class Iterator : IIterator
			{
			public:
				Iterator(Aggregate* aggregate)
					: m_Aggregate(aggregate)
				{
				}

				void First() override final
				{
					m_Index = 0;
				}

				void Next() override final
				{
					++m_Index;
				}

				bool End() const override final
				{
					return m_Index >= m_Aggregate->Size();
				}

				uint32_t Item() const override final
				{
					return (*m_Aggregate)[m_Index];
				}
			protected:
				Aggregate* m_Aggregate = nullptr;
				uint32_t m_Index = 0u;
			};
		}

		namespace _Interpreter
		{
			class Context
			{
			};

			class IExpression
			{
			public:
				virtual void Interpret(Context* context) = 0;
			};

			class TerminalExpression : public IExpression
			{
			public:
				void Interpret(Context* context) override final
				{
				}
			};

			class NonterminalExpression : public IExpression
			{
			public:
				void Interpret(Context* context) override final
				{
				}
			};
		}
	}
}

namespace ProducerConsumer
{
	namespace OneOnOne
	{
		template<class T>
		class Storage
		{
		public:
			void Store(const T* item)
			{
				std::lock_guard<std::mutex> locker(m_Storage.first);

				m_Storage.second.push(item);
			}

			T* Fetch()
			{
				std::lock_guard<std::mutex> locker(m_Storage.first);

				auto item = m_Storage.second.front();
				m_Storage.second.pop();

				return item;
			}
		protected:
			std::pair<std::mutex, std::queue<T*>> m_Storage;
		};

		template <class T>
		class IProducer
		{
		public:
			virtual T* Produce() = 0;

			void Run()
			{
				while (true)
				{
					Storage<T> storage;
					storage.Store(Produce());
				}
			}
		};

		template <class T>
		class IConsumer
		{
		public:
			virtual void Consume(T* product) = 0;

			void Run()
			{
				while (true)
				{
					Storage<T> storage;
					Consume(storage.Fetch());
				}
			}
		};
	}
}

NAMESPACE_END(Gear)
