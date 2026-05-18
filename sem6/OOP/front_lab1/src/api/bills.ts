import apiClient from './client'

export interface Bill {
    id: number
    orderId: number
    totalAmount: number
    status: string
    createdAt: string
}

export const getBillByOrderId = async (orderId: number): Promise<Bill> => {
    const response = await apiClient.get('/bills', { params: { orderId } })
    return response.data
}

export const createBill = async (orderId: number): Promise<Bill> => {
    const response = await apiClient.post('/bills', null, { params: { orderId } })
    return response.data
}

export const payBill = async (billId: number): Promise<void> => {
    await apiClient.put(`/bills/${billId}/pay`)
}