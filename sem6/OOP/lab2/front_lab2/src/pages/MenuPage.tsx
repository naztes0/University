import { useEffect, useState } from 'react'
import { getMenuItems } from '../api/menu'
import type { MenuItem } from '../api/menu'
import { createOrder } from '../api/orders'

function MenuPage() {
    const [items, setItems] = useState<MenuItem[]>([])
    const [cart, setCart] = useState<{ item: MenuItem; quantity: number }[]>([])
    const [tableNumber, setTableNumber] = useState<number>(1)
    const [loading, setLoading] = useState(true)
    const [success, setSuccess] = useState<string | null>(null)

    useEffect(() => {
        getMenuItems().then(setItems).finally(() => setLoading(false))
    }, [])

    const addToCart = (item: MenuItem) => {
        setCart(prev => {
            const existing = prev.find(c => c.item.id === item.id)
            if (existing) return prev.map(c => c.item.id === item.id ? { ...c, quantity: c.quantity + 1 } : c)
            return [...prev, { item, quantity: 1 }]
        })
    }

    const removeFromCart = (itemId: number) => {
        setCart(prev => prev.filter(c => c.item.id !== itemId))
    }

    const placeOrder = async () => {
        if (cart.length === 0) return
        try {
            await createOrder({ tableNumber, items: cart.map(c => ({ menuItemId: c.item.id, quantity: c.quantity })) })
            setCart([])
            setSuccess('Order placed successfully!')
            setTimeout(() => setSuccess(null), 3000)
        } catch {
            alert('Failed to place order')
        }
    }

    const total = cart.reduce((sum, c) => sum + c.item.price * c.quantity, 0)

    const categoryColors: Record<string, string> = {
        SOUP: '#fff3bf',
        MAIN: '#d3f9d8',
        SALAD: '#d3f9d8',
        DRINK: '#e7f5ff',
    }

    if (loading) return (
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '60vh' }}>
            <p style={{ color: '#868e96' }}>Loading menu...</p>
        </div>
    )

    return (
        <div style={{ maxWidth: '1200px', margin: '0 auto', padding: '32px 24px', display: 'flex', gap: '32px' }}>

            {/* Menu items */}
            <div style={{ flex: 1 }}>
                <h2 style={{ fontSize: '22px', fontWeight: 700, marginBottom: '20px' }}>Our Menu</h2>
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(240px, 1fr))', gap: '16px' }}>
                    {items.map(item => (
                        <div key={item.id} style={{
                            background: 'white',
                            borderRadius: '12px',
                            padding: '20px',
                            boxShadow: '0 2px 8px rgba(0,0,0,0.06)',
                            display: 'flex',
                            flexDirection: 'column',
                            gap: '8px',
                        }}>
                            <span style={{
                                background: categoryColors[item.category] || '#f1f3f5',
                                color: '#495057',
                                fontSize: '11px',
                                fontWeight: 600,
                                padding: '3px 10px',
                                borderRadius: '20px',
                                alignSelf: 'flex-start',
                                textTransform: 'uppercase',
                                letterSpacing: '0.5px',
                            }}>
                                {item.category}
                            </span>
                            <h3 style={{ fontSize: '16px', fontWeight: 600 }}>{item.name}</h3>
                            <p style={{ fontSize: '13px', color: '#868e96', flexGrow: 1 }}>{item.description}</p>
                            <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginTop: '8px' }}>
                                <span style={{ fontSize: '18px', fontWeight: 700, color: '#2f9e44' }}>₴{item.price}</span>
                                <button
                                    onClick={() => addToCart(item)}
                                    style={{ background: '#4dabf7', color: 'white', padding: '8px 16px', fontSize: '13px' }}
                                >
                                    + Add
                                </button>
                            </div>
                        </div>
                    ))}
                </div>
            </div>

            {/* Cart */}
            <div style={{ width: '320px', flexShrink: 0 }}>
                <div style={{
                    background: 'white',
                    borderRadius: '12px',
                    padding: '24px',
                    boxShadow: '0 2px 8px rgba(0,0,0,0.06)',
                    position: 'sticky',
                    top: '80px',
                }}>
                    <h3 style={{ fontSize: '18px', fontWeight: 700, marginBottom: '16px' }}>🛒 Your Order</h3>

                    <div style={{ display: 'flex', alignItems: 'center', gap: '10px', marginBottom: '16px' }}>
                        <span style={{ fontSize: '14px', color: '#495057' }}>Table:</span>
                        <input
                            type="number"
                            value={tableNumber}
                            onChange={e => setTableNumber(Number(e.target.value))}
                            min={1}
                            style={{ width: '70px' }}
                        />
                    </div>

                    {cart.length === 0 ? (
                        <p style={{ color: '#adb5bd', fontSize: '14px', textAlign: 'center', padding: '20px 0' }}>
                            Your cart is empty
                        </p>
                    ) : (
                        <>
                            {cart.map(c => (
                                <div key={c.item.id} style={{
                                    display: 'flex',
                                    justifyContent: 'space-between',
                                    alignItems: 'center',
                                    padding: '10px 0',
                                    borderBottom: '1px solid #f1f3f5',
                                }}>
                                    <div>
                                        <p style={{ fontSize: '14px', fontWeight: 500 }}>{c.item.name}</p>
                                        <p style={{ fontSize: '12px', color: '#868e96' }}>x{c.quantity} × ₴{c.item.price}</p>
                                    </div>
                                    <div style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
                                        <span style={{ fontSize: '14px', fontWeight: 600 }}>₴{(c.item.price * c.quantity).toFixed(2)}</span>
                                        <button
                                            onClick={() => removeFromCart(c.item.id)}
                                            style={{ background: '#fff5f5', color: '#c92a2a', padding: '4px 8px', fontSize: '12px' }}
                                        >
                                            ✕
                                        </button>
                                    </div>
                                </div>
                            ))}

                            <div style={{ display: 'flex', justifyContent: 'space-between', padding: '16px 0 0', fontWeight: 700, fontSize: '16px' }}>
                                <span>Total</span>
                                <span style={{ color: '#2f9e44' }}>₴{total.toFixed(2)}</span>
                            </div>

                            <button
                                onClick={placeOrder}
                                style={{ background: '#2f9e44', color: 'white', padding: '12px', width: '100%', fontSize: '15px', fontWeight: 600, marginTop: '12px' }}
                            >
                                Place Order
                            </button>
                        </>
                    )}

                    {success && (
                        <div style={{
                            background: '#d3f9d8',
                            color: '#2f9e44',
                            padding: '10px 14px',
                            borderRadius: '8px',
                            marginTop: '12px',
                            fontSize: '14px',
                            textAlign: 'center',
                        }}>
                            {success}
                        </div>
                    )}
                </div>
            </div>
        </div>
    )
}

export default MenuPage