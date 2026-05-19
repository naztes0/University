import { useEffect, useState } from 'react'
import { getOrders, updateOrderStatus } from '../api/orders'
import type { Order } from '../api/orders'
import { createBill } from '../api/bills'

const statusColors: Record<string, { bg: string; color: string }> = {
    NEW: { bg: '#e7f5ff', color: '#1971c2' },
    CONFIRMED: { bg: '#fff3bf', color: '#e67700' },
    IN_KITCHEN: { bg: '#ffe8cc', color: '#d9480f' },
    READY: { bg: '#d3f9d8', color: '#2f9e44' },
    BILLED: { bg: '#f3d9fa', color: '#862e9c' },
    PAID: { bg: '#d3f9d8', color: '#2f9e44' },
}

function AdminPage() {
    const [orders, setOrders] = useState<Order[]>([])
    const [loading, setLoading] = useState(true)

    useEffect(() => {
        getOrders().then(setOrders).finally(() => setLoading(false))
    }, [])

    const handleStatusUpdate = async (orderId: number, status: string) => {
        try {
            const updated = await updateOrderStatus(orderId, status)
            setOrders(prev => prev.map(o => o.id === orderId ? updated : o))
        } catch {
            alert('Failed to update status')
        }
    }

    const handleCreateBill = async (orderId: number) => {
        try {
            await createBill(orderId)
            setOrders(prev => prev.map(o => o.id === orderId ? { ...o, status: 'BILLED' } : o))
        } catch {
            alert('Failed to create bill')
        }
    }

    if (loading) return (
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '60vh' }}>
            <p style={{ color: '#868e96' }}>Loading...</p>
        </div>
    )

    return (
        <div style={{ maxWidth: '900px', margin: '0 auto', padding: '32px 24px' }}>
            <h2 style={{ fontSize: '22px', fontWeight: 700, marginBottom: '20px' }}>Admin Panel</h2>

            {orders.length === 0 ? (
                <div style={{ background: 'white', borderRadius: '12px', padding: '40px', textAlign: 'center', boxShadow: '0 2px 8px rgba(0,0,0,0.06)' }}>
                    <p style={{ color: '#adb5bd' }}>No orders</p>
                </div>
            ) : (
                <div style={{ display: 'flex', flexDirection: 'column', gap: '16px' }}>
                    {orders.map(order => {
                        const s = statusColors[order.status] || { bg: '#f1f3f5', color: '#495057' }
                        return (
                            <div key={order.id} style={{ background: 'white', borderRadius: '12px', padding: '24px', boxShadow: '0 2px 8px rgba(0,0,0,0.06)' }}>
                                <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '16px' }}>
                                    <div>
                                        <h3 style={{ fontSize: '16px', fontWeight: 700 }}>Order #{order.id}</h3>
                                        <p style={{ fontSize: '13px', color: '#868e96', marginTop: '2px' }}>Table {order.tableNumber}</p>
                                    </div>
                                    <span style={{ background: s.bg, color: s.color, padding: '4px 14px', borderRadius: '20px', fontSize: '13px', fontWeight: 600 }}>
                                        {order.status}
                                    </span>
                                </div>

                                <div style={{ borderTop: '1px solid #f1f3f5', paddingTop: '12px', marginBottom: '16px' }}>
                                    {order.items?.map(item => (
                                        <div key={item.menuItemId} style={{ display: 'flex', justifyContent: 'space-between', padding: '6px 0', fontSize: '14px' }}>
                                            <span>{item.menuItemName} x{item.quantity}</span>
                                            <span style={{ color: '#2f9e44', fontWeight: 500 }}>₴{item.priceAtOrder}</span>
                                        </div>
                                    ))}
                                </div>

                                <div style={{ display: 'flex', gap: '10px' }}>
                                    {order.status === 'NEW' && (
                                        <button
                                            onClick={() => handleStatusUpdate(order.id, 'CONFIRMED')}
                                            style={{ background: '#4dabf7', color: 'white', padding: '8px 20px', fontWeight: 600 }}
                                        >
                                            Confirm Order
                                        </button>
                                    )}
                                    {order.status === 'READY' && (
                                        <button
                                            onClick={() => handleCreateBill(order.id)}
                                            style={{ background: '#862e9c', color: 'white', padding: '8px 20px', fontWeight: 600 }}
                                        >
                                            Issue Bill
                                        </button>
                                    )}
                                </div>
                            </div>
                        )
                    })}
                </div>
            )}
        </div>
    )
}

export default AdminPage