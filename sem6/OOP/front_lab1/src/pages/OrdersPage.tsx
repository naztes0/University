import { useEffect, useState } from 'react'
import { getOrders } from '../api/orders'
import type { Order } from '../api/orders'
import { getBillByOrderId, payBill } from '../api/bills'

const statusColors: Record<string, { bg: string; color: string }> = {
    NEW: { bg: '#e7f5ff', color: '#1971c2' },
    CONFIRMED: { bg: '#fff3bf', color: '#e67700' },
    IN_KITCHEN: { bg: '#ffe8cc', color: '#d9480f' },
    READY: { bg: '#d3f9d8', color: '#2f9e44' },
    BILLED: { bg: '#f3d9fa', color: '#862e9c' },
    PAID: { bg: '#d3f9d8', color: '#2f9e44' },
}

function OrdersPage() {
    const [orders, setOrders] = useState<Order[]>([])
    const [loading, setLoading] = useState(true)

    useEffect(() => {
        getOrders().then(setOrders).finally(() => setLoading(false))
    }, [])

    const handlePay = async (orderId: number) => {
        try {
            const bill = await getBillByOrderId(orderId)
            await payBill(bill.id)
            setOrders(prev => prev.map(o => o.id === orderId ? { ...o, status: 'PAID' } : o))
        } catch {
            alert('Payment failed')
        }
    }

    if (loading) return (
        <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '60vh' }}>
            <p style={{ color: '#868e96' }}>Loading orders...</p>
        </div>
    )

    return (
        <div style={{ maxWidth: '800px', margin: '0 auto', padding: '32px 24px' }}>
            <h2 style={{ fontSize: '22px', fontWeight: 700, marginBottom: '20px' }}>My Orders</h2>

            {orders.length === 0 ? (
                <div style={{ background: 'white', borderRadius: '12px', padding: '40px', textAlign: 'center', boxShadow: '0 2px 8px rgba(0,0,0,0.06)' }}>
                    <p style={{ color: '#adb5bd', fontSize: '16px' }}>No orders yet</p>
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
                                        <p style={{ fontSize: '13px', color: '#868e96', marginTop: '2px' }}>
                                            Table {order.tableNumber} · {new Date(order.createdAt).toLocaleString()}
                                        </p>
                                    </div>
                                    <span style={{ background: s.bg, color: s.color, padding: '4px 14px', borderRadius: '20px', fontSize: '13px', fontWeight: 600 }}>
                                        {order.status}
                                    </span>
                                </div>

                                <div style={{ borderTop: '1px solid #f1f3f5', paddingTop: '12px' }}>
                                    {order.items?.map(item => (
                                        <div key={item.menuItemId} style={{ display: 'flex', justifyContent: 'space-between', padding: '6px 0', fontSize: '14px' }}>
                                            <span>{item.menuItemName} x{item.quantity}</span>
                                            <span style={{ color: '#2f9e44', fontWeight: 500 }}>₴{item.priceAtOrder}</span>
                                        </div>
                                    ))}
                                </div>

                                {order.status === 'BILLED' && (
                                    <button
                                        onClick={() => handlePay(order.id)}
                                        style={{ background: '#4dabf7', color: 'white', padding: '10px 24px', marginTop: '16px', fontWeight: 600 }}
                                    >
                                        Pay Bill
                                    </button>
                                )}
                            </div>
                        )
                    })}
                </div>
            )}
        </div>
    )
}

export default OrdersPage