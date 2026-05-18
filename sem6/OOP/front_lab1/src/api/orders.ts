import apiClient from './client'

export interface OrderItem {
    menuItemId: number
    menuItemName: string
    quantity: number
    priceAtOrder: number
}

export interface Order {
    id: number
    userId: number
    tableNumber: number
    status: string
    createdAt: string
    items: OrderItem[]
}

export interface CreateOrderRequest {
    tableNumber: number
    items: { menuItemId: number; quantity: number }[]
}

export const getOrders = async (status?: string): Promise<Order[]> => {
    const params = status ? { status } : {}
    const response = await apiClient.get('/orders', { params })
    return response.data
}

export const createOrder = async (request: CreateOrderRequest): Promise<Order> => {
    const response = await apiClient.post('/orders', request)
    return response.data
}

export const updateOrderStatus = async (id: number, status: string): Promise<Order> => {
    const response = await apiClient.put(`/orders/${id}`, { status })
    return response.data
}