import { useEffect, useState } from 'react'
import { getOrders, updateOrderStatus } from '../api/orders'
import type { Order } from '../api/orders'

function KitchenPage() {
    const [orders, setOrders] = useState<Order[]>([])
    const [loading, setLoading] = useState(true)

    useEffect(() => {
        Promise.all([getOrders('CONFIRMED'), getOrders('IN_KITCHEN')])
            .then(([confirmed, inKitchen]) => setOrders([...confirmed, ...inKitchen]))
            .finally(() => setLoading(false))
    }, [])

    const handleStatusUpdate = async (orderId: number, status: string) => {
        try {
            const updated = await updateOrderStatus(orderId, status)
            setOrders(prev => prev.map(o => o.id === orderId ? updated : o))
        } catch {
            alert('Failed to update status')
        }
    }

    if (loading) return (
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '60vh' }}>
            <p style={{ color: '#868e96' }}>Loading...</p>
        </div>
    )

    return (
        <div style={{ maxWidth: '900px', margin: '0 auto', padding: '32px 24px' }}>
            <h2 style={{ fontSize: '22px', fontWeight: 700, marginBottom: '20px' }}>Kitchen Board</h2>

            {orders.length === 0 ? (
                <div style={{ background: 'white', borderRadius: '12px', padding: '40px', textAlign: 'center', boxShadow: '0 2px 8px rgba(0,0,0,0.06)' }}>
                    <p style={{ color: '#adb5bd', fontSize: '16px' }}>No active orders 🎉</p>
                </div>
            ) : (
                <div style={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fill, minmax(280px, 1fr))', gap: '16px' }}>
                    {orders.map(order => (
                        <div key={order.id} style={{
                            background: order.status === 'IN_KITCHEN' ? '#fff9db' : 'white',
                            borderRadius: '12px',
                            padding: '24px',
                            boxShadow: '0 2px 8px rgba(0,0,0,0.06)',
                            border: order.status === 'IN_KITCHEN' ? '2px solid #ffd43b' : '2px solid transparent',
                        }}>
                            <div style={{ display: 'flex', justifyContent: 'space-between', marginBottom: '12px' }}>
                                <h3 style={{ fontSize: '16px', fontWeight: 700 }}>Order #{order.id}</h3>
                                <span style={{ fontSize: '13px', color: '#868e96' }}>Table {order.tableNumber}</span>
                            </div>

                            <div style={{ marginBottom: '16px' }}>
                                {order.items?.map(item => (
                                    <div key={item.menuItemId} style={{ display: 'flex', justifyContent: 'space-between', padding: '6px 0', fontSize: '14px', borderBottom: '1px solid #f1f3f5' }}>
                                        <span style={{ fontWeight: 500 }}>{item.menuItemName}</span>
                                        <span style={{ color: '#868e96' }}>x{item.quantity}</span>
                                    </div>
                                ))}
                            </div>

                            {order.status === 'CONFIRMED' && (
                                <button
                                    onClick={() => handleStatusUpdate(order.id, 'IN_KITCHEN')}
                                    style={{ background: '#fd7e14', color: 'white', padding: '10px', width: '100%', fontWeight: 600 }}
                                >
                                    🍳 Start Cooking
                                </button>
                            )}
                            {order.status === 'IN_KITCHEN' && (
                                <button
                                    onClick={() => handleStatusUpdate(order.id, 'READY')}
                                    style={{ background: '#2f9e44', color: 'white', padding: '10px', width: '100%', fontWeight: 600 }}
                                >
                                    ✅ Mark as Ready
                                </button>
                            )}
                        </div>
                    ))}
                </div>
            )}
        </div>
    )
}

export default KitchenPage